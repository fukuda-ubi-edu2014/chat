#
# Copyright (c) 2014 Fukuda Laboratory and Shigemi ISHIDA, Kyushu University
#
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

# primary target
.PHONY: all debug lib
all: lib
	$(MAKE) $(DB_FLAG) -C server

lib:
	$(MAKE) -C lib

# debug target
debug: DB_FLAG =debug
debug: all

clean:
	$(MAKE) clean -C server

cleanup:
	$(MAKE) cleanup -C server
