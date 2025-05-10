#!/usr/bin/perl

use strict;

sub make_regexp($)
{
	my $str = shift;
	$str =~ s/\\/\\\\/g;
	$str =~ s/\"/\\\"/g;
	return $str;
}

sub make_data($)
{
	my $str = shift;
	$str =~ s/\\(?![n|r])/\\\\/g;
	$str =~ s/\"/\\\"/g;
	return $str;
}

sub make_result_str($)
{
	my $str = shift;
	$str =~ s/\\(?![n|r])/\\\\/g;
	$str =~ s/\"/\\\"/g;
	return $str;
}

sub make_replace_pattern($)
{
	my $str = shift;
	$str =~ s/\\/\\\\/g;
	$str =~ s/\"/\\\"/g;
	return $str;
}

sub make_result($)
{
	my $data = shift;
	if($data eq 'y') {
		return 1;
	}
	return 0;
}

my $st_name = $ARGV[0];
printf("struct perl_re_test_st ${st_name}[] = {\n");

my $txt_line = 0;
my @data;
while(<STDIN>) {
	$txt_line++;

	chomp();
	next if($_ eq '');
	next if($_ =~ /^#/);

	@data = split(/\t/);

	next if($data[0] =~ /^\'/);
	next if($data[0] =~ /^\//);
	next if($data[1] eq '-');
	next if($data[3] =~ /\$[\-+]\[\d+\]+/);

	# yet support function
	next if($data[0] =~ /\\R/);
	next if($data[0] =~ /\\N/);
	next if($data[0] =~ /\\c/);
	next if($data[0] =~ /\\h/);
	next if($data[0] =~ /\\H/);
	next if($data[0] =~ /\\v/);
	next if($data[0] =~ /\\V/);
	next if($data[0] =~ /\\[bB]\{(?:gcb|g|lb|sb|wb)\}/);
	next if($data[0] =~ /\[\[:(?:cntrl|graph):\]\]/);
	next if($data[0] =~ /\Q[[:^xdigit:]]\E/);
	next if($data[0] =~ /\Q(?m)\E/);
	next if($data[0] =~ /\Q(*PRUNE)\E/);
	next if($data[0] =~ /\Q(*THEN)\E/);
	next if($data[0] =~ /\Q(*FAIL\E/);
	next if($data[0] =~ /\Q(*F)\E/);
	next if($data[0] =~ /\Q(*ACCEPT\E/);

	printf("\t{\"%s\", \"%s\", %s, \"%s\", \"%s\", %d, __LINE__}, \n",
		make_regexp($data[0]),
		make_data($data[1]),
		make_result($data[2]),
		make_replace_pattern($data[3]),
		make_result_str($data[4]),
		$txt_line);
}

printf("};\n");

