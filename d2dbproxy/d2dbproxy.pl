#!/usr/bin/perl

use strict;
use warnings;

use POSIX;
use IO::Socket::INET;
use IO::Select;
use Socket;
use Fcntl;
use HTTP::Request;
use HTTP::Response;
use Data::Dumper;
use URI;
use URI::QueryParam;
use Net::IP::Match::XS;
use File::Temp qw/ tempfile /;
use Config::ApacheFormat;

use constant {
    
  D2GS_D2DBS_SAVE_DATA_REQUEST => 0x30,
  D2DBS_D2GS_SAVE_DATA_REPLY => 0x30,
  
  D2DBS_SAVE_DATA_SUCCESS => 0,
  D2DBS_SAVE_DATA_FAILED => 1,
  
  D2GS_D2DBS_GET_DATA_REQUEST => 0x31,  
  D2DBS_D2GS_GET_DATA_REPLY => 0x31,
  
  D2GS_D2DBS_CHAR_LOCK => 0x33,
  
  D2DBS_D2GS_ECHOREQUEST => 0x34,
  D2GS_D2DBS_ECHOREPLY => 0x34,
    
  D2DBS_GET_DATA_SUCCESS => 0,
  D2DBS_GET_DATA_FAILED => 1,
  D2DBS_GET_DATA_CHARLOCKED => 2,
  
  D2GS_DATA_CHARSAVE => 0x01,  
  
};

my $config = Config::ApacheFormat->new();
eval {
	$config->read("config.cfg")
};
if ($@) {
	warn "WARN: Config load failed: $@";
}

my $d2slibpath = ($config->get("D2slibPath") or "../src/tools");

unless ( -x "$d2slibpath/d2stoxml" && 
		 -x "$d2slibpath/d2sdropitem" && 
		 -x "$d2slibpath/d2spickupitem") {
		 	die("FATAL: Directory '$d2slibpath' does not contain executable d2slib tools.");		 	
		 }
		 
my @AllowedIP = $config->get("AllowedIP");
@AllowedIP = ("127.0.0.1") if ($#AllowedIP == -1);

$SIG{'INT'} = 'CLEANUP';

my $dbs = IO::Socket::INET->new(
  PeerAddr => ($config->get("D2dbsHost") or "127.0.0.1"),
  PeerPort => ($config->get("D2dbsPort") or 6114),
  Proto => 'tcp',
  Blocking => 0  
) or die("brak polaczenia z D2DBS\n");

my $http = IO::Socket::INET->new(Listen => 15,
 LocalPort => ($config->get("HttpPort") or 18800),
 Proto => 'tcp',
 Blocking => 0
) or die("Can not run HTTP server: $@\n");

warn "INFO: OK let's do my job.\n";

$dbs->send(pack("c", 0x65));
my $select = IO::Select->new();

$select->add($dbs);
$select->add($http);

my $seqno = 1;
my %dbsreq = ();
my %httpreq = ();
my %httpbuf = ();

my $dbsbuff = '';

my $dbspkglen = 0;
my $dbspkgtype = 0;
my $dbspkgseq = 0;

my %output = ();

while (1) {

  foreach my $sock ($select->can_read(1)) {
  
    if ($sock == $dbs) {
      my $data = '';
      my $rv = $sock->recv($data, 8192, 0);
      unless (defined($rv) && length $data) {
        die("FATAL: d2dbs die and me too :(");
      }
      $dbsbuff .= $data;
      handle_dbs_data($data);  
    } elsif ($sock == $http) {
      my $client = $http->accept();
      
      unless (match_ip($client->peerhost, @AllowedIP)) {
        warn "INFO: kick new unauth connection from ".$client->peerhost."\n";
        $client->shutdown(2);
        $client->close();
      } else {
        $client->blocking(0);
        warn "INFO: new http connection from ".$client->peerhost."\n";
        $select->add($client);
      }
      
    } else {
      my $data = '';
      my $rv = $sock->recv($data, 8192, 0);
            
      unless (defined($rv) && length $data) {
        $select->remove($sock);
        delete $httpbuf{$sock};
        delete $httpreq{$sock};
        close $sock;
        next;       
      } 
      
      $httpbuf{$sock} .= $data;
      
      if (length($httpbuf{$sock}) > 2000) {
        $select->remove($sock);
        delete $httpbuf{$sock};
        delete $httpreq{$sock};
        close $sock;
        next;
      }
      
      handle_http_data($sock);      
    }
  }
  
  foreach my $sock ($select->can_write(1)) {
    next unless exists $output{$sock};
    my $rv = $sock->send($output{$sock}, 0);

    unless (defined $rv) {
      warn "WARN: what? you say that i can write...\n";
      next;
    }

    substr($output{$sock}, 0, $rv) = '';
    delete $output{$sock} unless length $output{$sock};
    
  }
}

sub handle_dbs_data {
  my $data = shift;
  
  if ($dbspkglen == 0 && length($data) >= 8) {
    ($dbspkglen, $dbspkgtype, $dbspkgseq) = unpack('vvV', substr($data, 0, 8));
    $dbspkglen -= 8;
    substr($data, 0, 8) = '';    
    
    warn sprintf("INFO: Got DBS packet header: len=%d, type=0x%02x, seq=%d\n", $dbspkglen, $dbspkgtype, $dbspkgseq);
    
  }
  
  if (length($data) >= $dbspkglen) {
    if ($dbspkgtype == D2DBS_D2GS_ECHOREQUEST) {    
    
      $output{$dbs} .= pack("vvV", 8, D2GS_D2DBS_ECHOREPLY, $dbspkgseq);
      warn "INFO: Ping - Pong\n";
      
    } elsif ($dbspkgtype == D2DBS_D2GS_GET_DATA_REPLY) {
    
      my ($result, $chartime, $allowlader, $datatype, $datalen) = unpack('VVVvv', substr($data, 0, 16));
      substr($data, 0, 16) = '';
      $dbspkglen -= 16;
      
      my $char = substr($data, 0, index($data, "\0"));
      $dbspkglen -= index($data, "\0")+1;
      substr($data, 0, index($data, "\0")+1) = '';
      
      if (exists($dbsreq{$dbspkgseq})) {
      
        if ($result == D2DBS_GET_DATA_SUCCESS) {
          
          $dbsreq{$dbspkgseq}{'callback'}->(
            $dbsreq{$dbspkgseq}{'sock'}, 
            $dbsreq{$dbspkgseq}{'acc'}, 
            $dbsreq{$dbspkgseq}{'char'}, 
            $dbsreq{$dbspkgseq}{'realm'}, 
            $data,
            $dbsreq{$dbspkgseq}{'params'});
          
        } elsif ($result == D2DBS_GET_DATA_FAILED) {
          $output{$dbsreq{$dbspkgseq}{'sock'}} .= build_response('<error>Get character data failed.</error>');
        } elsif ($result == D2DBS_GET_DATA_CHARLOCKED) {
          $output{$dbsreq{$dbspkgseq}{'sock'}} .= build_response('<error>Character is locked</error>');
        } else {
          $output{$dbsreq{$dbspkgseq}{'sock'}} .= build_response('<error>Unknown get character result</error>');
        }
        
      }
      
      my $pkg = pack('Va*xa*xa*x', 0, $dbsreq{$dbspkgseq}{'acc'}, $dbsreq{$dbspkgseq}{'char'}, $dbsreq{$dbspkgseq}{'realm'});
      my $seq = send_dbs_packet(D2GS_D2DBS_CHAR_LOCK, $pkg);
    
    } elsif ($dbspkgtype == D2DBS_D2GS_SAVE_DATA_REPLY) {
      my ($result, $datatype, $charname) = unpack('Vva*', $data);
      
      if ($result == D2DBS_SAVE_DATA_SUCCESS) {
        $dbsreq{$dbspkgseq}{'callback'}->(
          $dbsreq{$dbspkgseq}{'sock'}, 
          $dbsreq{$dbspkgseq}{'acc'},  
          $dbsreq{$dbspkgseq}{'char'}, 
          $dbsreq{$dbspkgseq}{'realm'},
          $charname,
          $dbsreq{$dbspkgseq}{'params'}
        );
                                                                                                                                                        
      } elsif ($result == D2DBS_SAVE_DATA_FAILED) {
        $output{$dbsreq{$dbspkgseq}{'sock'}} .= build_response('<error>Save character failed.</error>');
      } else {
        $output{$dbsreq{$dbspkgseq}{'sock'}} .= build_response('<error>Unknown save character result.</error>');
      }
                                      
    } else {
      warn "WARN: Unknown packet!\n";
    }
    
    substr($data, 0, $dbspkglen) = '';
    
    if (exists($dbsreq{$dbspkgseq})) {
      delete $dbsreq{$dbspkgseq};
    }
    
    $dbspkglen = $dbspkgtype = $dbspkgseq = 0;
    
  }
}

sub handle_http_data {
  my $sock = shift;
  
  return if ($httpbuf{$sock} !~ /\r\n\r\n$/);
  
  my $req = HTTP::Request->parse($httpbuf{$sock});
  delete $httpbuf{$sock};
  
  if (
    $req->uri->path ne '/getcharacter' &&
    $req->uri->path ne '/dropitem' && 
    $req->uri->path ne '/pickupitem'
  ) {
    $output{$sock} .= build_response("<error>Unknown Command</error>");
    return;
  }
  
  my $acc = $req->uri->query_param("acc");    
  my $char = $req->uri->query_param("char");  
  my $realm = $req->uri->query_param("realm");
  
  my $key = $req->uri->query_param("key");
  my $item = $req->uri->query_param("item");
  
  if (length($acc) > 16 || length($char) > 16 || length($realm) > 16 ||
      length($acc) < 1 || length($char) < 1 || length($realm) < 1) {
        $output{$sock} .= build_response("<error>Bad data</error>");
    return;
  }
  
  if ($req->uri->path eq '/dropitem' && length($key) < 1) {
    $output{$sock} .= build_response("<error>Bad data</error>");
    return;    
  }
  
  if ($req->uri->path eq '/pickupitem' && length($item) < 1) {
    $output{$sock} .= build_response("<error>Bad data</error>");
    return;
  }
    
  my $seq = send_dbs_packet(D2GS_D2DBS_GET_DATA_REQUEST, pack('va*xa*xa*x', D2GS_DATA_CHARSAVE, $acc, $char, $realm));
  
  $dbsreq{$seq} = { 
    'sock' => $sock,
    'acc' => $acc,  
    'char' => $char,
    'realm' => $realm,
  };                              
  
  if ($req->uri->path eq '/getcharacter') {
    $dbsreq{$seq}{'callback'} = \&getcharacter_callback;
    $dbsreq{$seq}{'params'} = ();
  }
  
  if ($req->uri->path eq '/dropitem') {
    $dbsreq{$seq}{'callback'} = \&dropitem_callback; 
    $dbsreq{$seq}{'params'} = ( $key );
  }
  
  if ($req->uri->path eq '/pickupitem') {
    $dbsreq{$seq}{'callback'} = \&pickupitem_callback; 
    $dbsreq{$seq}{'params'} = ( $item );
  }
  
}

sub build_response {
  my $content = shift;
  my $res = HTTP::Response->new(200, "OK");
  $res->header("Content-Type" => "application/xml; charset=iso-8859-1");
  $res->header("Connection" => "close");
  $res->header("Content-Length" => length $content);
  $res->content($content);
  
  return "HTTP/1.0 ".$res->as_string;
  
}

sub send_dbs_packet {
  my ($type, $pkg) = @_;
  
  $output{$dbs} .= pack("vvV", 8+length($pkg), $type, $seqno);
  $output{$dbs} .= $pkg;
  
  return $seqno++;
  
}

sub getcharacter_callback {
  my ($sock, $acc, $char, $realm, $data) = @_;
  
  my ($tempfh, $tempfilename) = tempfile(UNLINK => 0, SUFFIX => '.d2s');
  binmode $tempfh;
  print $tempfh $data;
  close $tempfh;
  
  my $cmd = $d2slibpath . '/d2stoxml';
  my $xml = `$cmd $tempfilename`;
  
  $output{$sock} .= build_response($xml);
  unlink $tempfilename;

}

sub dropitem_callback {
  my ($sock, $acc, $char, $realm, $data, $key) = @_;
  
  my ($tempfh, $tempfilename) = tempfile(UNLINK => 0, SUFFIX => '.d2s');
  binmode $tempfh;
  print $tempfh $data;
  close $tempfh;

  $key = int($key);
  my $cmd = $d2slibpath . '/d2sdropitem';
  my $xml = `$cmd $tempfilename $key`;
  
  if ($xml =~ /<error>/) {  
    $output{$sock} .= build_response($xml);
    unlink $tempfilename;
    return;
  }
  
  $data = getfilecontent($tempfilename);
  
  my $pkg = pack('vva*xa*xa*x', D2GS_DATA_CHARSAVE, length($data), $acc, $char, $realm);
  $pkg .= $data;
  my $seq = send_dbs_packet(D2GS_D2DBS_SAVE_DATA_REQUEST, $pkg);
  
  $dbsreq{$seq} = {
    'sock' => $sock,
    'acc' => $acc,  
    'char' => $char,
    'realm' => $realm,
    'callback' => \&save_callback,
    'params' => ( $xml ),
  };
  
}

sub pickupitem_callback {
  my ($sock, $acc, $char, $realm, $data, $item) = @_;
  
  my ($tempfh, $tempfilename) = tempfile(UNLINK => 0, SUFFIX => '.d2s');
  binmode $tempfh;
  print $tempfh $data;
  close $tempfh;
     
  $item =~ s/[^a-zA-Z0-9+\/=]//g;
  
  my $cmd = $d2slibpath . '/d2spickupite';
  my $xml = `$cmd $tempfilename $item`;
  
  if ($xml =~ /<error>/) {  
    $output{$sock} .= build_response($xml);
    unlink $tempfilename;
    return;
  }
  
  $data = getfilecontent($tempfilename);
  
  my $pkg = pack('vva*xa*xa*x', D2GS_DATA_CHARSAVE, length($data), $acc, $char, $realm);
  $pkg .= $data;
  my $seq = send_dbs_packet(D2GS_D2DBS_SAVE_DATA_REQUEST, $pkg);

  $dbsreq{$seq} = {
    'sock' => $sock,
    'acc' => $acc,  
    'char' => $char,
    'realm' => $realm,
    'callback' => \&save_callback,
    'params' => ( $xml ),
  };        
  
} 

sub save_callback {
  my ($sock, $acc, $char, $realm, $data, $xml) = @_;
  $output{$sock} .= build_response($xml);
} 

sub getfilecontent {
  my $fn = shift;
  
  local $/ = undef;
  open FILE, $fn;
  binmode FILE;  
  my $data = <FILE>;
  close FILE;
  
  return $data;
}
                      
    
sub CLEANUP {  
  warn "\n\nCaught Interrupt (^C), Aborting\n";     
  
  $select->remove($http);
  $http->close;

  foreach my $sock ($select->handles) {
    $select->remove($sock);
    $sock->shutdown(2);
    $sock->close;    
  }
  
  exit(1);
}
                              