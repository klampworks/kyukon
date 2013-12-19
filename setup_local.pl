#!/usr/bin/perl

$path = '/var/www/localhost/htdocs/';
$dl_path = $path . "dl/";

`rm -rf $path*`;
`mkdir -p $dl_path`;

open $in, '>', $path . "index.html";

for ($i = 0; $i < 10; $i++) {

	my $name = `cat /usr/share/dict/propernames | shuf | head -1`;	
	print $in "$name";
	
	my $new_dir = $dl_path . $name;
	`dd count=10 if=/dev/urandom of=$new_dir`;
}

my $f = $dl_path . "manifest";
`md5sum $dl_path* > $f`;

`chmod -R 777 $path`;
`chown -R lighttpd:lighttpd $path`;
close $in;
