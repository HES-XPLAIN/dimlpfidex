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
- Lists must be inbetween ""
- booleans in uppurcase or inbetween "" ? Verify

### Datafiles format
Categorisation of attributes (ex : Smoker/No_Smoker) -> where to include this? It's data preprocessing...

### CNN
Tell how to add a new model (add in argument model choices and add the model in the code, if not 2D (1D, 3D, etc. change pair_type of original_input_size and model_input_size to a list_type to accept more or less than 2 values))
Indiquer qu'il est important de diminuer la taille des images au préalable...

### Normalization
- Topo sur la normalisation (importance, quand l'utiliser et quand ne pas...) -> où en parler ? Dans le fichier normalization c'est suffisant ? Pas sûr...

### Import
Import des algos (from dimlpfidex.dimlp import dimlpBT etc. -> l'indiquer à un autre endroit que dans usage example?)
