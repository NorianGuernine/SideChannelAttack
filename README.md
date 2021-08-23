# SideChannelAttack

This is a cryptanalysis project where I use CPA and DPA attacks to break the encryption key of an AES encryption.

## How to use it

### Select your dataset.

I used the traces-avr-aes128-vcc-newkey.zip dataset for this project.

You can download it on this page: https://app.assembla.com/spaces/chipwhisperer/wiki/Example_Captures
There are 10,000 traces made up of 3,000 points each:
```
traces.shape
(10001, 3000)
```
This is what a trace looks like:

![Trace](https://github.com/NorianGuernine/SideChannelAttack/blob/main/Pictures/Trace.png)

### Launch main.py

Then launch the file main.py and click on "ouvrir" in the frame "Enregistrement des traces" and select the traces.

After that click on "ouvrir" in the frame "enregistrement des plaintexts" and select the plaintext file.

Then select the type of attack in the "Choix de l'attaque" box.
You have the choice between DPA and CPA algorithm.

Choose the number of traces to be used in the "Nombre de traces" box.

Finally, launch the attack by clicking on "Casser la clé":

![dechiffrement](https://github.com/NorianGuernine/SideChannelAttack/blob/main/Pictures/dechiffrement.png)

At the end of the process you can retrieve the key in the "clé retrouvée (base 10)" field:

![cletrouve](https://github.com/NorianGuernine/SideChannelAttack/blob/main/Pictures/cletrouve.png)

We can verify the correct functioning of the program thanks to the file "2013.12.08-14.38.21_knownkey.npy" which contains the key used for the encryption:

```
key=np.load(r'2013.12.08-14.38.21_knownkey.npy')
key
array([212, 129, 234, 233, 215,  81,  45,  89,  84,   8, 234, 119, 246,
        48, 176, 195], dtype=uint8)
```
The key we found is correct.

