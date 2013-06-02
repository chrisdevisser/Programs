Options:
  -? [ --help ]                 Shows this page.
  -t [ --title ] arg (=Message) Sets the title.
  -i [ --icon ] arg             Sets the icon. Valid values are warning, info, 
                                question, and error.
  -b [ --button ] arg           Sets the buttons. Valid values are 
                                abortretryignore, canceltrycontinue, help, ok, 
                                okcancel, retrycancel, yesno, and yesnocancel.
  -d [ --default ] arg          Sets the default button. Valid values are 1, 2,
                                and 3.
  -m [ --modal ] arg            Sets the modality. Valid values are app, 
                                system, and task.
  -j [ --right-justify ]        Right-justifies the text.
  -r [ --rtl ]                  Displays text from right to left.
  -f [ --foreground ]           Makes the message box the foreground window.
  -p [ --topmost ]              Makes the message box always on top.
  -e [ --time ] arg (=60000)    Makes the message box disappear after a number 
                                of milliseconds.
  -s [ --timestamp ]            Shows a timestamp in YYYY-MM-DD HH:MM:SS format
                                below the message.


Displays a message box with the attributes specified.
Slashes also work for short versions.
Newlines may be inserted with \n.

Sample Usages
=============
mbox hi
mbox /t "Hello world"
mbox /p Fatal error /i error
mbox /j right-justified text
mbox Would you like to quit? /b yesno /d 2

The return value is how the message box was closed. It can be one of the
following values:

 0 - There was an error or help was displayed.
 1 - The ok button was pressed.
 2 - The cancel button was pressed or the message box was cancelled.
 3 - The abort button was pressed.
 4 - The retry button was pressed.
 5 - The ignore button was pressed.
 6 - The yes button was pressed.
 7 - The no button was pressed.
 8 - The message box timed out.
 9 - The help button was pressed.
 10 - The try again button was pressed.
 11 - The continue button was pressed.
