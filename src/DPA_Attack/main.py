from tkinter import *
from tkinter import ttk
import tkinter.filedialog
from SideChannel import *

def Ouverture_fichier_cle():
    try:
        filename = tkinter.filedialog.askopenfilename(title="Ouvrir fichier traces",filetypes=[('numpy files','.npy'),('all files','.*')])
        path_traces.set(filename)
    except FileNotFoundError:
        pass

def Ouverture_fichier_pt():
    try:
        filename = tkinter.filedialog.askopenfilename(title="Ouvrir fichier traces",filetypes=[('numpy files','.npy'),('all files','.*')])
        path_plain.set(filename)
    except FileNotFoundError:
        pass


def Attaque():
    cle.set('')
    Att = SideChannelAttack(nombre.get(),path_traces.get(),path_plain.get())
    for i in range(1,17):
        if ChoixAttaque.get() == "CPA":
            key=Att.CPA(i-1,i)
        elif ChoixAttaque.get() == "DPA":
            key=Att.DPA(i-1,i)
        progress['value'] = 6.25*i
        cle.set(key)
        Frame1.update_idletasks()                
    
# Création de la fenêtre principale
fenetre = Tk()
fenetre.title("Attaques par canaux cachés")

# création d'un widget Frame pour cle dans la fenêtre principale 
Frame1 = Frame(fenetre,width=500, height=500, borderwidth=2,relief=GROOVE)
Frame1.pack(side=TOP,padx=20,pady=20)

# Création d'un widget Label (texte 'Enregistrement des traces')
Label_cle = Label(Frame1, text = 'Enregistrement des traces')
Label_cle.grid(columnspan = 4)

# Création d'un widget Entry (chemin des traces)
path_traces= StringVar()
Champ = Entry(Frame1,textvariable= path_traces,width = 40)
Champ.focus_set()
Champ.grid(row=1,column=0,padx =0, pady =10)

# Création d'un widget Button (bouton Ouvrir)
Ouvrir_cle = Button(Frame1, text ='Ouvrir', command = Ouverture_fichier_cle)
Ouvrir_cle.grid(row=1,column=1,padx =0, pady =10)

# Création d'un widget Label (texte 'Enregistrement des plaintext')
Label_cle = Label(Frame1, text = 'Enregistrement des plaintexts')
Label_cle.grid(columnspan = 4)

# Création d'un widget Entry (chemin des plaintext)
path_plain= StringVar()
Champ = Entry(Frame1,textvariable= path_plain,width = 40)
Champ.focus_set()
Champ.grid(row=3,column=0,padx =0, pady =10)

# Création d'un widget Button (bouton Ouvrir)
Ouvrir_pt = Button(Frame1, text ='Ouvrir', command = Ouverture_fichier_pt)
Ouvrir_pt.grid(row=3,column=1,padx =0, pady =10)

# Création d'un widget Label (texte 'Choix de l'attaque')
Label_choix = Label(Frame1, text = "Choix de l'attaque")
Label_choix.grid(columnspan = 4)

#Choix d'attaque
ChoixAttaque = ttk.Combobox(Frame1,values=["DPA","CPA"])
ChoixAttaque.grid(row=7,columnspan = 4,padx =0, pady =5)

# Création d'un widget Label (texte 'Choix de l'attaque')
Label_choix = Label(Frame1, text = "Nombre de traces")
Label_choix.grid(columnspan = 4)

#Nombre de traces
nombre = StringVar()
nombre.set(10000)
NB_TRACES = Spinbox(Frame1,from_=0,to=10000,increment=1,textvariable=nombre)
NB_TRACES.grid(row=9,columnspan = 4,padx =0, pady =5)

# Création d'un widget Button (bouton 'Casser la clé')
cassage = Button(Frame1, text ='Casser la clé', command = Attaque)
cassage.grid(row=11,columnspan = 4,padx =0, pady =5)

#Bar de progression
progress = ttk.Progressbar(Frame1, orient = HORIZONTAL,
              length = 400, mode = 'determinate')
progress.grid(row=13,columnspan = 4,padx =0, pady =5)

# Création d'un widget Label (texte 'clé retrouvée')
Label_cle = Label(Frame1, text = 'Clé retrouvée (base 10)')
Label_cle.grid(columnspan = 4)

#Clé retrouvé (base 10)
cle= StringVar()
cle_retrouve = Entry(Frame1,textvariable= cle,width = 50)
cle_retrouve.focus_set()
cle_retrouve.grid(row=17,columnspan = 4,padx =0, pady =10)

#Montage fenêtre
fenetre.mainloop()
