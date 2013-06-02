Options:
  -? [ --help ]             Shows this message.
  -p [ --process ] arg      The name or PID of the process to jam. The PID is 
                            tried first. To make it only a name, specify the 
                            nopid option.
  -n [ --nopid ]            Specifies that the process option is a name. It 
                            will not be tried as a PID if the name fails.
  -t [ --threads ] arg (=1) The number of threads to inject.


Creates at least one thread in a process to make it use CPU.
jam.dll must be in the same directory.

Sample Usage
============

jam 1234
jam notepad
jam /nopid 5678
jam "process with spaces"
jam /t 2 notepad.exe

Returns 0 on success and 1 on failure.
