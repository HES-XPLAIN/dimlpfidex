# Temporary documentation
*Used to remember what to write in the future official documentation*

#### Files extension:
- No check on files names inputed by the user, its on the user responsibility to input filenames that are logic/sane/whatever

Advices :
- ruleFile : .rls
- predictionFile : .out
- weightFile : .wts
- images : .png
- other files : .txt

#### JSON templates:
- add JSON templates for every executable in order to be usable for users

#### Parameters:
- add a description of every parameter including its data type, its importance (mandatory or optional) and what it is used for

#### Add execution examples:
- For JSON and CLI

#### TO BE TESTED
- Test if a non-conventionnal path is handled (if an exception is thrown at runtime start) when **trying to use a linux path in a Windows env**

### Dimlp params
- If you want to train dimlp with an error or accuracy threshold stopping criterion and without beeing stopped by a number of iterations, you have to put a big number in nb_epochs.


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
- Lists must be inbetween ""
- booleans in uppurcase or inbetween "" ? Verify

### paramètres FidexGloRules :
train_file : données avec lesquelles tu as entrainé ton modèle

### Explain every statistics
