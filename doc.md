# Temporary documentation
*Used to remember what to write in the future official documentation*

#### JSON templates:
- Explain how to write a list type (with "")

#### TO BE TESTED
- Test if a non-conventionnal path is handled (if an exception is thrown at runtime start) when **trying to use a linux path in a Windows env**

### Note in case performance decrease for fidexGloRules
- We have moved  the "return false" in fidexAlgo after the rule extraction and stats computation. We did it for Fidex but it could have a computational cost.

### Launch fidex from Python library :
- Need to include the library before :
from dimlpfidex import fidex or from dimlpfidex import dimlp
If using Windows :
import os
dir_path = os.path.abspath('dimlpfidex')
os.add_dll_directory(dir_path)
- root_folder is not bin but it is where we launch Python
- Lists must be inbetween "" (Ah bon?->voir comment les écrire comme d'autres types bizarre genre les dict)
- booleans in uppurcase or inbetween "" ? Verify
