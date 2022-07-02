#!/usr/bin/perl

# ***********************************************************************

#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#  
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#  
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#  
#  
#   @file    status_script.pl
#   @author  Sebastian Lesse
#   @date    2022 / 04 / 02
#   @brief   Short description of this file

# ***********************************************************************

use Time::HiRes qw(sleep);

# ***********************************************************************

## operation mode set new status
use constant MODE_SET => 1;

# operation mode get actual status
use constant MODE_GET => 2;

# ***********************************************************************

# Major version
my $version_major = 1;

# Minor version
my $version_minor = 2;

# ***********************************************************************

# the higher the value the more output is generated
my $verbose_level = 0;

# ***********************************************************************

# file to use for status-inforamtion
my $status_file = "NULL";

# status to write into the status-file
my $status_line = "NULL";

# mode of operation
# 0 : undifined
# 1 : set status
# 2 : get status
my $op_mode = 0;

# ***********************************************************************

# prints the given message into the actual log-file
# a new line is added autoamtically to the end of the message
sub log_msg {
    my ($msg) = @_;
    system("printf \"%s\n\" \"$msg\" >> $log_file");
}

# ***********************************************************************

# prints a message on the console if the verbose-level is high enugh
# using &print_ms(3, "Message to print");
sub print_msg {
    my ($level, $msg) = @_;
    print "$msg\n" if $verbose_level >= $level;
}

# ***********************************************************************

# pauses the program execution for the given amount of milliseconds
sub pause_ms {

    my ($time_ms) = @_;
}

# creates the file at the given path
sub file_create {

    my ($file) = @_;
    &print_msg(3, "CREATE FILE: $file");
}

# ***********************************************************************

# writes the given string into the given file
# all content of the file is overwritten
# if the file is not available nothing happens
sub file_write {

    my ($file, $line) = @_;
    &print_msg(3, "WRITE FILE:$file - LINE:$line");

    open(FH_STATUS_FILE, ">", $file)    or die "OPEN FILE:$file FAILED\n";
    #flock(FH_STATUS_FILE, LOCK_EX)      or die "LOCK FILE:$file FAILED\n";
    print(FH_STATUS_FILE "$line\n")     or die "WRITE FILE:$file FAILED\n";
    close(FH_STATUS_FILE)               or die "CLOSE FILE:$file FAILED\n";
}

# ***********************************************************************

# reads the content of the given file and returns it
# content means the first line if the file
# all other lines are ignored
sub file_read {

    my ($file) = @_;
    my $line = "";

    &print_msg(3, "READ FILE: $file - LINE:$line");

    open(FH_STATUS_FILE, "<", $file)    or die "OPEN FILE:$file FAILED\n";
    #flock(FH_STATUS_FILE, LOCK_EX)      or die "LOCK FILE:$file FAILED\n";
    $line = <FH_STATUS_FILE>;
    close(FH_STATUS_FILE)               or die "CLOSE FILE:$file FAILED\n";

    print("$line");
}

# ***********************************************************************

# prints help and usage information
sub print_help {

    print ("\n\n");
    print ("STATUS-SCRIPT v$version_major.$version_minor\n");
    print ("\n");
    print ("\tUsage: ./status_script.pl <OPTIONS>\n");
    print ("\n");
    print ("OPTIONS:\n");
    print ("\n");

    print ("\t-file:<PATH-TO-FILE> \t File to use for status information\n");
    print ("\t\t\t\t EXAMPLE: -file:../status_file.txt\n\n");

    print ("\t-get \t\t\t reads the actual status from the given status-file (-file) \n");
    print ("\t\t\t\t and prints it on the console\n");
    print ("\t\t\t\t EXAMPLE: -get\n\n");

    print ("\t-set:\"<NEW-STATUS>\" \t writes the new status into the given status-file (-file)\n");
    print ("\t\t\t\t The exisitng status will be overwritten\n");
    print ("\t\t\t\t EXAMPLE: -set:\"NEW STATUS\"\n\n");

    print ("\t-verbose:<LEVEL> \t verbose-level, higher numbers cause more\n");
    print ("\t\t\t\t messages to be printed on console\n");
    print ("\t\t\t\t EXAMPLE: -verbose:5\n\n");

    print ("\n\n");
}

# ***********************************************************************

# get the arguments which are used calling this application
foreach my $argument (@ARGV) {

    $_ = $argument;

    if (/-help/i) {
        &print_help();
        exit 0;
    }

    if (/-verbose:(\d)/i) {
        $verbose_level = $1;
        &print_msg(3, "ARGUMENT verbose = $1");
    }

    if (/-get/i) {
        &print_msg(3, "ARGUMENT get");
        $op_mode = MODE_GET;
    }

    if (/-set:(.*)/i) {
        &print_msg(3, "ARGUMENT set = $1");
        $op_mode = MODE_SET;
        $status_line = $1;
    }

    if (/-file:(.*)/i) {
        &print_msg(3, "ARGUMENT file = $1");
        $status_file = $1;
    }
}

# ***********************************************************************

&print_msg(1, "\n\nSTATUS-SCRIPT v$version_major.$version_minor");
&print_msg(1, "----------------------\n");

# ***********************************************************************

if ($status_file eq "NULL") {
    print "ERROR - no file given\n\n ";
    exit 1;
}

if ($op_mode eq MODE_SET) {

    if ($status_line eq "NULL") {
        print "ERROR - no status given\n\n ";
        exit 1;
    }

    &file_write($status_file, $status_line);
}

if ($op_mode eq MODE_GET) {
    &file_read($status_file);
}

# ***********************************************************************

&print_msg(3, "PROGRAMM FINISHED");
exit 0;
