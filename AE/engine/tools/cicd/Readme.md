
	<< Client / View >>
	-

	<< Server >>
	-| wait for git push in stable branch
	-| wait for user command
	- send build command
	- wait for build artifacts
	- send test command
	- wait for test artifacts

	<< Build Machine >>
	- register in server
	- wait for build command (repository, commit)
	- build dbg, dev, prof, rel
	- send log
	- upload exe/apk/elf

	<< Test Machine >>
	- register in server
	- wait for test command
	- download exe/apk/elf
	- run tests:
		- run single test
		- send log
		- send artifacts
	- run perf tests:
		- run single test
		- send log


