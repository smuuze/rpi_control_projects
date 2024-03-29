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
#   @file    start_music_radio.pl
#   @author  Sebastian Lesse
#   @date    2022 / 03 / 26
#   @brief   performs a build of projects within a given directory
#            You need to isntall the following packaged via apt to
#            to run this script on a Raspberry Pi.
#            - libfile-copy-recursive-perl

# ***********************************************************************

use File::Path qw(rmtree);
use File::Copy::Recursive qw(dircopy );

# ***********************************************************************

# Major version
my $version_major = 1;

# Minor version
my $version_minor = 1;

# ***********************************************************************

# the higher the value the more output is generated
my $verbose_level = 0;

# ***********************************************************************

# address to the framework-repository
my $repository_frmwrk = "";

# address to the projects-repository
my $repository_proj = "";

# path to use for the operation of this script
my $working_path = "INVALID";

# path to the logfile that will be generated during this process
# is automatically created by this script
my $log_file = "build_log.txt";

# path to the destination directory where the projects release are stored
my $destination = "NULL";

# path to the source directory where the compiled program is located
# all data of this directory will be copied to the destination
my $source_dir = "NULL";

# the lsit of branches given by the user
my @list_of_branches = ();

# command to call on beginning of the build-script
my $start_script = "NULL";

# command to call on exiting the build-script
my $stop_script = "NULL";

# ***********************************************************************

# get the actual date and time of the system this script is executed from
sub get_date {
    my $date = localtime();
}

# ***********************************************************************

my $LOG_HANDLE;
my @log_file_content;

sub log_open {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);

    $year += 1900;
    $mday = sprintf("%02d", $mday);
    $mon = sprintf("%02d", ($mon + 1));

    $log_file = "build_log_${year}_${mon}_${mday}.txt";
}

# prints the given message into the actual log-file
# a new line is added autoamtically to the end of the message
sub log_write {
    my ($msg) = @_;
    my $date = &get_date();
    my $line = "$date | $msg\n";

    push(@log_file_content, $line);
    print $line if $verbose_level > 0;
}

sub log_close {
    open($LOG_HANDLE, ">>", $log_file) or die "Open $log_file has failed!";
    print($LOG_HANDLE @log_file_content);
    close($LOG_HANDLE);
}

# ***********************************************************************

sub fatal_exit {
    my ($msg) = @_;
    &log_close();
    die $msg;
}

# ***********************************************************************

# executes the given command
sub execute_command {
    my ($cmd) = @_;

    &log_write("Running command: $cmd");

    # 2>&1 - pipe STDERR out to STDOUT
    open(CH, "$cmd 2>&1 |") or &fatal_exit("Open $cmd has failed!");

    while (<CH>) {
        chomp;
        &log_write("$_");
    }

    close(CH);

    my $return_value = $?;
    $return_value;
}

# ***********************************************************************

# clones the given repository into the current working directory
sub git_clone {

    my ($repo) = @_;
    my $cmd = "git clone $repo";

    &execute_command($cmd);
}

# ***********************************************************************

# switches to the branch of the actual repository
sub git_switch_branch {

    my ($branch) = @_;
    my $cmd = "git checkout $branch";

    &execute_command($cmd);
}

# ***********************************************************************

# deletes the given repository and its content
sub delete_directory {
    my ($dir) = @_;

    &log_write("Delete directory: $dir");
    rmtree($dir) or &fatal_exit("DELETE DIRECTORY($dir) FAILED ");
}

# ***********************************************************************

# change actual working directory to the given one
sub change_directory {
    my ($dir) = @_;

    &log_write("Change directory: $dir");
    chdir($dir) or &fatal_exit("CHANGE DIRECTORY($dir) FAILED ");
}

# ***********************************************************************

# gets the name of a repository web-address in the format
# https://www.gibhub.com/<account>/<my_repository_name>.git
sub get_repository_name {
    ($_) = @_;
    my $repo_name = "";

    if (/https:\/\/www\..*\.com\/.*\/(.*)\.git/i) {

        $repo_name = $1;
        &log_write("Repository-Name: = $repo_name");

    } elsif (/https:\/\/www\..*\.com\/.*\/(.*)/i) {

        $repo_name = $1;
        &log_write("Repository-Name: = $repo_name");

    } else {

        $repo_name = "INVALID";
    }

    $repo_name; # return value
}

# ***********************************************************************

# builds a project that has a makefile
sub build_project {
    ($dir) = @_;

    # check if makefile is existing
    if (-e "makefile" || -e "Makefile") {

        &log_write("--- BUILD START ---");

        $cmd = "make clean";
        if (&execute_command($cmd) != 0) {
            &log_write("Build $dir has failed!");
            return -2;
        }

        $cmd = "make load_frmwrk_revision";
        if (&execute_command($cmd) != 0) {
            &log_write("Build $dir has failed!");
            return -1;
        }

        $cmd = "make release";
        if (&execute_command($cmd) != 0) {
            &log_write("Build $dir has failed!");
            return -3;
        }

        &log_write("--- BUILD END ---");

        return 0;

    } else {

        &log_write("Skip directory: $dir - no makefile");
        return -4;
    }
}

# ***********************************************************************

# parses a comma-separated list of branches that is given via a string
# in the format branch1,branch2,branch3
sub get_list_of_branches {

    my ($csv_list_string) = @_;

    $_ = $csv_list_string;
    my @branch_list = ();

    if (/^(\w+)$/gi) { # i - ignore case
        push(@branch_list, $1);
    } else {
        while (/(\w+),/gi) { # i - ignore case
            push(@branch_list, $1);
        }

        if (/,(\w+)$/gi) { # i - ignore case
            push(@branch_list, $1);
        }
    }

    return @branch_list;
}

# copies all data from src to dest
# if dest is not existing, it will be created
# src and dest are expected to be a directory each
sub copy_result {
    my ($src, $dest) = @_;

    &log_write("COPY: $src -> $dest");

    File::Copy::Recursive::dircopy($src, $dest) or &fatal_exit($!);
}

# ***********************************************************************

# prints help and usage information
sub print_help {

    print ("\n\n");
    print ("BUILD-SCRIPT v$version_major.$version_minor\n");
    print ("\n");
    print ("\tUsage: ./build_script.pl <OPTIONS>\n");
    print ("\n");
    print ("OPTIONS:\n");
    print ("\n");

    print ("\t-verbose:<LEVEL> \t\t\t verbose-level, higher numbers cause more\n");
    print ("\t\t\t\t\t\t messages to be printed on console\n");
    print ("\t\t\t\t\t\t EXAMPLE: -verbose:5\n\n");

    print ("\t-repository:<REPOSITORY-ADRESS> \t Web-address to the projects repository\n");
    print ("\t\t\t\t\t\t Will be cloned next to the projects repository\n");
    print ("\t\t\t\t\t\t EXAMPLE: -repository:https://www.github.com/<ACCOUNT>/<REPOSITORY>\n\n");

    print ("\t-framework:<REPOSITORY-ADDRESS> \t Web-address to the framework repository\n");
    print ("\t\t\t\t\t\t Will be cloned next to the projects repository\n");
    print ("\t\t\t\t\t\t EXAMPLE: -framework:https://www.github.com/<ACCOUNT>/<REPOSITORY>\n\n");

    print ("\t-path:<WORKING-DIRECTORY> \t\t path to the directory were the script will\n");
    print ("\t\t\t\t\t\t load the repositories for processing\n");
    print ("\t\t\t\t\t\t EXAMPLE: -path:/tmp/build_script\n\n");

    print ("\t-log:<LOG-FILE-PATH> \t\t\t path to the log-file where the build-output\n");
    print ("\t\t\t\t\t\t and script-process is printed into\n");
    print ("\t\t\t\t\t\t EXAMPLE: -log:/var/log/build_script.log\n\n");

    print ("\t-destination:<DESTINATION-DIRECTORY>\t path to the directory where the compilation\n");
    print ("\t\t\t\t\t\t results will be copied to\n");
    print ("\t\t\t\t\t\t EXAMPLE: -destination:/mnt/file_server/share1\n\n");

    print ("\t-source:<SOURCE-DIRECTORY> \t\t directory where the compilation results are stored inside of the project directory (e.g. release)\n");
    print ("\t\t\t\t\t\t EXAMPLE: -source:release\n\n");

    print ("\t-start:\"<START-SCRIPT>\" \t\t Full path to a script that is executed before the build-process.\n");
    print ("\t\t\t\t\t\t Arguments can also be given\n");
    print ("\t\t\t\t\t\t EXAMPLE: -start:\"my_start_script -arg\"\n\n");

    print ("\t-stop:\"<STOP-SCRIPT>\" \t\t\t Full path to a script that is executed after the build-process.\n");
    print ("\t\t\t\t\t\t Arguments can also be given\n");
    print ("\t\t\t\t\t\t EXAMPLE: -stop:\"my_stop_script -arg\"\n\n");

    print ("\t-branches:<LIST-OF-BRANCHES> \t\t comma separated list of branches that are used\n");
    print ("\t\t\t\t\t\t in the form <BRANCH1>,<BRANCH2>,<BRANCH3\n");
    print ("\t\t\t\t\t\t EXAMPLE: -branches:dev,master,debug");

    print ("\n\n");
}

# ***********************************************************************

&log_open();

# ***********************************************************************

# get the arguments which are used calling this application
foreach my $argument (@ARGV) {

    $_ = $argument;

    if (/-help/i) {

        &print_help();
        exit 0;

    } elsif (/-verbose:(\d)/i) {

        $verbose_level = $1;
        &log_write("ARGUMENT verbose = $1");

    } elsif (/-repository:(.*)/i) {

        &log_write("ARGUMENT repository = $1");
        $repository_proj = $1;

    } elsif (/-framework:(.*)/i) {

        &log_write("ARGUMENT framework = $1");
        $repository_frmwrk = $1;

    } elsif (/-path:(.*)/i) {

        &log_write("ARGUMENT path = $1");
        $working_path = $1;

    } elsif (/-destination:(.*)/i) {

        &log_write("ARGUMENT destination = $1");
        $destination = $1;

    } elsif (/-source:(.*)/i) {

        &log_write("ARGUMENT source = $1");
        $source_dir = $1;

    } elsif (/-start:(.*)/i) {

        &log_write("ARGUMENT start = $1");
        $start_script = $1;

    } elsif (/-stop:(.*)/i) {

        &log_write("ARGUMENT stop = $1");
        $stop_script = $1;

    } elsif (/-branches:(.*)/i) {

        &log_write("ARGUMENT branches = $1");
        @list_of_branches = &get_list_of_branches($1);

    } else {

        print("Invalid argument: \"$argument\"\nType -help for available arguments\n");
        exit 0;
    }
}

# ***********************************************************************

&log_write("\n\n========================================================\nBUILD-SCRIPT v$version_major.$version_minor\n========================================================");

# ***********************************************************************

if ($destination eq "NULL") {
    print "ERROR - argument -destination invalid\n\n ";
    exit;
}

if ($source_dir eq "NULL") {
    print "ERROR - argument -source invalid\n\n ";
    exit;
}

$branch_count = @list_of_branches;
if ($branch_count eq 0) {
    print "ERROR - argument -branches invalid\n\n ";
    exit;
}

if ($working_path eq "INVALID") {
    print "ERROR - argument -path invalid\n\n ";
    exit;
}

# ***********************************************************************

my $repository_name_proj = &get_repository_name($repository_proj);
if ($repository_name_proj eq "INVALID") {
    print "ERROR - invalid projects-repository given - $repository_proj\n\n ";
    exit;
}

my $repository_name_frmwrk = &get_repository_name($repository_frmwrk);
if ($repository_name_frmwrk eq "INVALID") {
    print "ERROR - invalid framework-repository given - $repository_frmwrk\n\n ";
    exit;
}

# ***********************************************************************

# running start-script
if ($start_script ne "NULL") {
    &execute_command($start_script);
}

# go into working directory
&change_directory($working_path);
&log_write("Set Working directory to: $working_path");

# clone framework repository
&log_write("Cloning Framework-Repository - $repository_name_frmwrk");
&git_clone($repository_frmwrk);

# clone projects-repository
&log_write("Cloning Projects-Repository - repository_name_proj")
&git_clone($repository_proj);

# ***********************************************************************

#go into projects-repository
&change_directory($repository_name_proj);

# get list of sub-directories
@dir_list = glob("*");

foreach my $branch (@list_of_branches) {

    &log_write("Switching to branch:  $branch");
    &log_write("Switching to branch:  $branch");

    &git_switch_branch($branch);
    &change_directory("..");

    &change_directory($repository_name_frmwrk);

    &git_switch_branch($branch);
    &change_directory("..");

    &change_directory($repository_name_proj);

    foreach my $dir (@dir_list) {

        # check if object is file
        if (-d $dir) {

            &change_directory($dir);

            if (&build_project() == 0) {
                &copy_result("$source_dir", "$destination/$dir/$branch");
            }
            
            &change_directory("..");
            &git_switch_branch($branch);

        } else {
            &log_write("Skip non-directory: $dir");
        }
    }
}

# ***********************************************************************

## go back to parent directory
&change_directory("..");

# delete the directories of the projects- and framework-repository
&delete_directory($repository_name_proj);
&delete_directory($repository_name_frmwrk);

# running start-script
if ($stop_script ne "NULL") {
    &execute_command($stop_script);
}

# ***********************************************************************

&log_write("PROGRAMM FINISHED\n\n\n");
&log_close();

exit 0;
