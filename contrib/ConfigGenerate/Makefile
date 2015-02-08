#
#   Copyright (C) 2015 Jetty
#
#   This file is part of HDMI-Light
#
#   HDMI-Light is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 2 of the License, or
#   (at your option) any later version.
#
#   HDMI-Light is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this code (see the file names COPING).
#   If not, see <http://www.gnu.org/licenses/>.
#

CC=gcc
CFLAGS=
LIBS= -lm

all: ConfigGenerate

ConfigGenerate: ConfigGenerate.c
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f ConfigGenerate
