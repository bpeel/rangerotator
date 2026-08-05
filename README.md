# Range Rotator

Range Rotator is a tiny LibreOffice Calc extension to provide a command to rotate the data in the current selection by 180°.

It would probably make more sense to implement this as a macro in a high-level language. The code here is intended to be used just to compare performance in a bug report about how slow this is.

## Building

Make sure you have the LibreOffice SDK installed. On Fedora this is in the `libreoffice-sdk` package. If you’re building LibreOffice from source make sure you have the `--enable-odk` in your `autogen.input`.

There should be a script called `setsdkenv_unix`. On Fedora this is installed in `/usr/lib64/libreoffice/sdk`. If you haven’t already, run the script and answer all of the configuration questions. This will create another script in your home directory which should be something like `~/libreoffice26.2_sdk/yourhostname/setsdkenv_unix.sh`. Source the script into your current shell environment like this:

```bash
source "$HOME/$(cd ~ && ls -d libreoffice??.?_sdk -t | head -n 1)/$HOSTNAME/setsdkenv_unix.sh"
```

Now you should be able to build the extension by just running `make`. This should also magically install the extension onto your LibreOffice installation.

## Using

Once the extension is installed the Range Rotator can be found found in the “Tools→AddOn” menu of any Calc document. If you click that menu item it then it will operate on the current selection.
