#!/usr/bin/perl -w

$literal_count = 0;
$literal_bits = 0;

$last_copy_code = 0;
$last_distance_code = 0;

$copy_count = 0;
$copy_bits = 0;

# init counts table
%counts = ();
for($copy_code = 257; $copy_code <= 285; ++$copy_code) {
	$counts{$copy_code} = {};
	for($dist_code = 0; $dist_code <= 29; ++$dist_code) {
		$counts{$copy_code}{$dist_code} = 0;
	}
}
	
while(<>) {
	if(/^([01]+)\s+literal/) {
		$literal_count += 1;
		$literal_bits += length($1);
	} elsif(/\#\s+(\d+) extra bits/) {
		$copy_bits += $1;
	} elsif(/^([01]+)\s+copy code: (\d+)/) {
		$last_copy_code = $2;
		$copy_bits += length($1);
	} elsif(/^([01]+)\s+distance code: (\d+)/) {
		$last_distance_code = $2;
		$copy_bits += length($1);

		$copy_count += 1;
		$counts{$last_copy_code}{$last_distance_code} += 1;
	}
}

$n = $literal_count - ($literal_bits / 8);
print "$literal_bits bits in $literal_count literals = $n bytes saved\n";
$n = $copy_bits / (8*$copy_count);
print "$copy_bits bits in $copy_count copies = $n bytes each\n";
print "\n";

# print header row
for($dist_code = 0; $dist_code <= 29; ++$dist_code) {
	print ",$dist_code";
}
print "\n";

# print counts table with header column 0
for($copy_code = 257; $copy_code <= 285; ++$copy_code) {
	print $copy_code;
	for($dist_code = 0; $dist_code <= 29; ++$dist_code) {
		$c = $counts{$copy_code}{$dist_code};
		if(0 == $c) {
			print ",";
		} else {
			print ",$c";
		}
	}
	print "\n";
}

print "\n";

