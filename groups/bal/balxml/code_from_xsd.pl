#!/usr/bin/env perl

use warnings;
use strict;
use English "-no_match_vars";

# Do not run this script in the original source directory.  Create a separate
# directory and copy this script and balxml.xsd into it, then run this script
# in that directory with no arguments.

# Then copy the file(s) that you want updated to the original source directory.

# You will have to hand-edit 'balxml_encodeingstyle.{h,cpp}' to add versions
# of the 'enum' values with 'BAEXML_' and 'e_' prefixes that match the original
# 'enum' values, modify all 'case' statements based on those enums to reflect
# the fact that the 'BAEXML_' and 'e_' enums are not different values but just
# copies of the original values.

# This file generates all the components described by balxml.xsd which, at the
# time of this writing, is

#: o balxml_decoderoptions
#: o balxml_encoderoptions
#: o balxml_encodingstyle

my $scriptName = "code_from_xsd.pl";
my $pkg = "balxml";
my $xsdfile = "$pkg.xsd";
my @generatedComponents = ("encodingstyle", "encoderoptions", "decoderoptions",
                                                               "configschema");
my $tmp = "tmp.$scriptName.$$.txt";
my $date = `date`;
chomp $date;
my $year = `date +%Y`;
chomp $year;
my $ret = 0;

my $apacheCopyright = "\n" .
"// ----------------------------------------------------------------------------\n" .
"// Copyright $year Bloomberg Finance L.P.\n" .
"//\n" .
"// Licensed under the Apache License, Version 2.0 (the \"License\");\n" .
"// you may not use this file except in compliance with the License.\n" .
"// You may obtain a copy of the License at\n" .
"//\n" .
"//     http://www.apache.org/licenses/LICENSE-2.0\n" .
"//\n" .
"// Unless required by applicable law or agreed to in writing, software\n" .
"// distributed under the License is distributed on an \"AS IS\" BASIS,\n" .
"// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n" .
"// See the License for the specific language governing permissions and\n" .
"// limitations under the License.\n" .
"// ----------------------------- END-OF-FILE ----------------------------------\n";

sub spillLines {
    # The argument is a list of lines that we read thinking it might be the
    # copyright message, but that turned out not to be.  'OUTFILE' has been
    # selected, so that where the output will go.

    my @lines = @_;

    while (0 < @lines) {
        shift @lines;

        print "$_";
    }
}

sub processFile {
    # Edit one file.

    my $sourcefile = $_[0];

    (-e $sourcefile) or die "$sourcefile not found";

    rename $sourcefile, $tmp;

    open INFILE,  "< $tmp";
    open OUTFILE, "> $sourcefile";
    select OUTFILE;

    my @lines;
    my $copyRightMode = 0;

    while (<INFILE>) {
        s/^#include <bdes_ident.h>$/#include <bsls_ident.h>/;
        s/^BDES_IDENT_RCSID/BSLS_IDENT_RCSID/;
        s/^BDES_IDENT_PRAGMA_ONCE$/BSLS_IDENT_PRAGMA_ONCE/;
        s!// close namespace BloombergLP!// close enterprise namespace!;
        if (m!^// GENERATED BY .*BAS_CODEGEN!) {
            print "// BAS_CODEGEN RUN BY $scriptName RUN ON $date\n";
        }

        if (0 == $copyRightMode && m!^// --*$!) {
            unshift @lines, $_;

            ++$copyRightMode;

            next;
        }
        elsif (1 == $copyRightMode) {
            unshift @lines, $_;

            if (!m!^//! && !/^$/) {
                $copyRightMode = 0;
                &spillLines(@lines);
                @lines = ();
            }
            elsif (m!^//.* Copyright \d\d\d\d Bloomberg!) {
                my ($ii, $line);
                my $maxLength = 20;

                # If it's all comments for less than $maxLength lines to EOF,
                # it's the copyright message at the end of the file to be
                # replaced.

                for ($ii = 0; $ii < $maxLength && ($line = <INFILE>); ++$ii) {
                    $_ = $line;
                    unshift @lines, $_;

                    if (!m!^//! && !m/^ *$/) {
			# The commented area ended and we are running into
			# real code.  Set $ii to $maxLength to signal failure.

                        $ii = $maxLength;
                        last;
                    }
                }

                if ($ii < $maxLength) {
                    print $apacheCopyright;
                    return;
                }
                else {
                    $copyRightMode = 0;
                    &spillLines(@lines);
                    @lines = ();
                }
            }

            next;
        }

        print $_;
    }

    print STDOUT "Unable to find copyright message in $sourcefile\n";
    $ret = 1;
}

if (`pwd -P` =~ m/.groups.bal.balxml$/) {
    die "Error: Do not run this script in the source directory, it\n" .
        "       will overwrite source files.  Run it in a separate\n" .
        "       directory and pick and choose the components you want.\n";
}

if (! -f $xsdfile) {
    die "Error: You must copy the file $xsdfile into the current\n" .
        "       directory before running this script.\n";
}

foreach my $component (@generatedComponents) {
    unlink glob "$pkg" . "_$component.*";
}

# EncodingStyle, EncoderOptions, DecoderOptions:

my $codegen = "bas_codegen.pl";
0 == system("$codegen -m msg -p $pkg -E --noExternalization" .
                       " --noAggregateConversion --noHashSupport $xsdfile") or
                    die "$codegen {de,en}coderoptions, encodingstyle failed\n";

# ConfigSchema file:

0 == system("$codegen -m cfg -p $pkg -E --noExternalization" .
                        " --noAggregateConversion --noHashSupport $xsdfile") or
                                          die "$codegen schemaconfig failed\n";

unlink $tmp;
foreach my $component (@generatedComponents) {
    foreach my $suffix (".h", ".cpp") {
        my $sourcefile = $pkg . "_" . $component . $suffix;
        print STDOUT "Modifying $sourcefile\n";

        &processFile($sourcefile);
    }
}

select STDOUT;

unlink $tmp;
exit $ret;
