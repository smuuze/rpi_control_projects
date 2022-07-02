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
#   @brief   Short description of this file

# ***********************************************************************

# Major version
my $version_major = 1;

# Minor version
my $version_minor = 1;

# ***********************************************************************

# the higher the value the more output is generated
my $verbose_level = 5;

# ***********************************************************************

# name of artist to play, is given via command line
my $artist_name;

# This file is sued to synchronize to instances of this script
# if the script is started a second time the first instance mus be stopped
# On starting a instance this file is created
# on runtime the instance will periodically check if this file is still available
# A second instance will delete an existing file
# The first instance will recognized that the file is gone and will stop itself
my $synchronize_file_name = "/home/shc/music_player_sync_file";

# Into this file the name of the actual playing track is stored
my $now_playing_file = "/home/shc/now_playing.txt";

# ***********************************************************************

# kills play processes to stop play music
sub kill_play {

    system("killall", "play");
    system("killall", "mpg3");
}

# subroutine to delete the synchroniozation file
# and to stop actual music playback
sub request_cancel {
    if (-e $synchronize_file_name) {

        print "Going to delete synchronize-file: ", $synchronize_file_name, "\n"
            if $verbose_level > 3;
        
        # the file exists and must be deleted
        unlink($synchronize_file_name);
    }

    &kill_play;

    # wait a short moment to exit script that was executed before
    system("sleep", "0.5");
}

# this routing reads the name of the actual playing track
sub get_actual_playing {

    if (-e $now_playing_file) {

        my $fh_now_playing;
        if (open($fh_now_playing, "<", $now_playing_file)) {

            my $line;
            if (defined($line = <$fh_now_playing>)){
                chomp($line);
                print($line);
            }

            close($fh_now_playing);
        }
    } else {
        print("-");
    }
}

# Sleeps for the given number of milliseconds
sub sleep {
    my ($duration) = @_;
    $duration = $duration / 1000;
    my $command = "sleep $duration";
    system($command);
}

# ***********************************************************************

# get the arguments which are used calling this application
foreach my $argument (@ARGV) {

    $_ = $argument;

    if (/-artist:(.*)/i) {
        $artist_name = $1;
    }

    if (/-verbose:(\d)/i) {
        $verbose_level = $1;
    }

    if (/-stop/i) {
        &request_cancel;
        exit;
    }

    if (/-actual/i) {
        &get_actual_playing;
        exit;
    }

    if (/-next/i) {
        &kill_play;
        exit;
    }
}

# ***********************************************************************

# into this file the shuffled playlist will be stored
my $playlist_file_name = "/home/shc/music_radio_playlist.m3u";

# command to generate a shuffled playlist
my $generate_playlist_command = "find /home/music/$artist_name/* -not -path '*/\\.*' -iname '*.mp3' -type f | shuf > $playlist_file_name";

# command to play a audio file
my $play_audio_file_command = "play";

print "Start Music Playback Script Version ", $version_major, ".", $version_minor, "\n"
    if $verbose_level > 3;

# delete the actual playing entry
my $fh_now_playing;
if (open($fh_now_playing, ">", $now_playing_file)) {
    print($fh_now_playing " ");
    close($fh_now_playing);
}

# Delete synchronizing file, if exisiting
&request_cancel;

my $fh_synchronization_file;
open($fh_synchronization_file, ">", $synchronize_file_name)
    or die "CREATE SYNC FILE FAILED";
close($fh_synchronization_file);

# Delete playlist-file, if exisiting
if (-e $playlist_file_name) {

    print "Going to delete playlist-file: ", $playlist_file_name, "\n"
        if $verbose_level > 3;

    unlink($playlist_file_name);
}

# delete music-files, if existing
if (-e $music_file_1) {

    print "Going to delete music-file: ", $music_file_1, "\n"
        if $verbose_level > 3;

    unlink($music_file_1);
}

print "Generate playlist-file ", $music_file_1, " from artist ", $artist_name, "\n"
    if $verbose_level > 3;

# generate playlist-file
print "Command: ", $generate_playlist_command, "\n"
    if $verbose_level > 4;

system($generate_playlist_command);

my $exit_requested = 0;

# loop until the porgramm is requested to be killed
unless ($exit_requested) {

    # Read Playlist file line by line
    my $fh_playlist_file;
    open($fh_playlist_file, "<", $playlist_file_name) # open file for read only
        or die("COULD NOT OPEN FILE: $playlist_file_name");

    # loop until the end of the playlsit file has been reached
    while ( defined($line = <$fh_playlist_file>) ) {

        chomp($line); # remove newline

        $_ = $line;

        # add \ in front of a whitespace
        if ( ! s/\s/\\ /g) {
            print ("SUB has FAILED - WHITE SPACE")
                if $verbose_level > 1;
        }

        # add \ in front of a opening-clamp
        if ( ! s/\(/\\(/g) {
            print ("SUB has FAILED - OPENING CLAMP")
                if $verbose_level > 1;
        }

        # add \ in front of a closing-clamp
        if ( ! s/\)/\\)/g) {
            print ("SUB has FAILED - CLOSING CLAMP")
                if $verbose_level > 1;
        }

        # add \ in front of a '
        if ( ! s/\'/\\'/g) {
            print ("SUB has FAILED - > ' <")
                if $verbose_level > 1;
        }

        # add \ in front of a .
        if ( ! s/\./\\./g) {
            print ("SUB has FAILED - > ' <")
                if $verbose_level > 1;
        }

        my $play_cmd_argument = $_;

        print("FILE: ", $line, "\n")
            if $verbose_level > 3;

        # remove file path and mp3 extension
        $_ = $line;
        if (/\/[0-9]?[0-9]?\s?-?\s?([^\/]*).mp3/i) {
            #print("TRACK: ", $1, "\n");
        }

        # remove leading numberes
        $_ = $1;
        if (/[0-9]?[0-9]?-?\s?(.*)/i) {
            #print("TRACK: ", $1, "\n");
        }

        # remove more leading numbers
        $_ = $1;
        if (/\/[0-9,-]*([a-z,A-Z,0-9,-, ,_,-,\,,']*)/i) {
            #print("TRACK: ", $1, "\n");
        }

        my $actual_track = $_;
        print("TRACK: ", $actual_track, "\n")
            if $verbose_level > 3;

        # store the title of the actual playing music-file
        if (open($fh_now_playing, ">", $now_playing_file)) {
            print($fh_now_playing $actual_track);
            close($fh_now_playing);
        }

        # now lets prepare the command
        $_ = $line;
        my $play_cmd = "$play_audio_file_command $play_cmd_argument &";

        print "PLAY COMMAND: ", $play_cmd, "\n"
            if $verbose_level > 3;

        # run play command with artist as argument
        # this will block until the play programm
        # has finisehd or was killed
        system($play_cmd);

        # wait until play command has finsihed
        my $play_still_running = 1;

        while ($play_still_running ) {

            &sleep(1000);

            if ( ! `top -b -n 1 | grep play`) {
                print("PLAY HAS STOPPED \n");
                $play_still_running = 0;
            }
        }

        # check if synchronization file is still existing
        if ( ! -e $synchronize_file_name) {
            print "CANCEL REQUESTED\n"
                if $verbose_level > 3;
                
            $exit_requested = 1;
            last;
        }
    }

    close($fh_playlist_file);
}

unlink($playlist_file_name);
unlink($now_playing_file);

print "PROGRAMM FINISHED\n"
    if $verbose_level > 3;

exit 0;
