#!/usr/bin/env python3

# https://www.mindat.org/strunz.php

import os, sys, json, re
import subprocess
import tkinter
import tkinter.ttk
import tkinter.filedialog
import tkinter.messagebox
import PIL.Image, PIL.ImageTk
import ttkthemes

class AutoScrollbar(tkinter.ttk.Scrollbar):
    def set(self, lo, hi):
        if float(lo) <= 0.0 and float(hi) >= 1.0:
            self.tk.call("grid", "remove", self)
        else:
            self.grid()
        tkinter.ttk.Scrollbar.set(self, lo, hi)
    def pack(self, **kw):
        raise TclError("cannot use pack with this widget")
    def place(self, **kw):
        raise TclError("cannot use place with this widget")

class MineralApp(tkinter.ttk.Frame):

    fields = [ 'Species', 'Number', 'UID', 'Class', 'Chemical Formula', 'Locality', 'Acquisition', 'Fluorescence', 'Radioactivity', 'Size', 'Weight', 'Price', 'Comments' ]
    minerals = {}
    selected = None
    max_fig_size = 400.0
    sort_by = 'Class'
    image_path = None
    uid_fmt = '%C_%S_%N'

    def __init__(self, parent, *args, **kwargs):
        #self.read_nickel_strunz()
        tkinter.ttk.Frame.__init__(self, parent, *args, **kwargs)
        self.winfo_toplevel().title("MineralApp")
        self.set_icon()
        self.style = ttkthemes.ThemedStyle(self)
        self.current_style = 'plastik'
        self.style.theme_use(self.current_style)
        self.parent = parent
        self.parent.rowconfigure(0, weight=0)
        self.parent.rowconfigure(1, weight=1)
        self.parent.columnconfigure(0, weight=1)
        self.top_panel = tkinter.ttk.Frame(self.parent)
        self.top_panel.grid(row=0, column=0, padx=0, pady=0, sticky='news')
        self.top_panel.columnconfigure(0, weight=1)
        self.top_panel.columnconfigure(8, weight=1)
        self.button_load = tkinter.ttk.Button(self.top_panel, text="Load database", command=self.read_from_file).grid(row=0, column=1, padx=5, pady=5)
        self.button_save = tkinter.ttk.Button(self.top_panel, text="Save database", command=self.save_to_file).grid(row=0, column=2, padx=5, pady=5)
        self.button_add_mineral = tkinter.ttk.Button(self.top_panel, text="Add new mineral", command=self.add_mineral).grid(row=0, column=3, padx=5, pady=5)
        self.button_modify = tkinter.ttk.Button(self.top_panel, text="Modify selected", command=self.modify_selected).grid(row=0, column=4, padx=5, pady=5)
        self.button_modify = tkinter.ttk.Button(self.top_panel, text="Export", command=self.export_report).grid(row=0, column=5, padx=5, pady=5)
        self.button_settings = tkinter.ttk.Button(self.top_panel, text="Settings", command=self.settings).grid(row=0, column=6, padx=5, pady=5)
        self.button_help = tkinter.ttk.Button(self.top_panel, text="Help me!", command=self.helpme).grid(row=0, column=7, padx=5, pady=5)
        self.button_quit = tkinter.ttk.Button(self.top_panel, text="Quit", command=self.parent.destroy).grid(row=0, column=8, padx=5, pady=5)
        self.show_minerals()
        self.update_view()

    def set_icon(self):
        dirs = [ os.getcwd(), os.path.dirname(__file__), '.', '..', 'icon', '../icon/' ]
        for d in dirs:
            p = os.path.join(d, 'icon-512.png')
            print('Testing', p)
            if os.path.isfile(p):
                print('Opening image', p)
                img = PIL.ImageTk.PhotoImage(PIL.Image.open(p))
                self.winfo_toplevel().tk.call('wm', 'iconphoto', self.winfo_toplevel()._w, img)
                return
        print('No icon image found.')

    def settings(self):
        root = tkinter.Toplevel(self.parent)
        root.title('Settings')
        root.columnconfigure(0, weight=1)
        root.rowconfigure(0, weight=1)
        window = tkinter.ttk.Frame(root)
        window.grid(row=0, column=0, sticky='news')
        window.columnconfigure(1, weight=1)

        # Sort option
        def set_sort(event):
            self.sort_by = sort_combo.get()
            self.update_view()
        sort_label = tkinter.ttk.Label(window, width=15, text='Sort by:', anchor='w')
        sort_label.grid(row=0, column=0, padx=5, pady=5)
        sort_values = ['Class', 'Number']
        sort_combo = tkinter.ttk.Combobox(window)
        sort_combo['values'] = sort_values
        sort_combo.current(sort_values.index(self.sort_by))
        sort_combo.grid(column=1, row=0)
        sort_combo.bind("<<ComboboxSelected>>", set_sort)

        # Windows style
        def set_style(event):
            self.current_style = style_combo.get()
            self.style.theme_use(self.current_style)
        style_label = tkinter.ttk.Label(window, width=15, text='Style:', anchor='w')
        style_label.grid(row=1, column=0, padx=5, pady=5)
        style_combo = tkinter.ttk.Combobox(window)
        style_combo['values'] = self.style.theme_names()
        style_combo.current(self.style.theme_names().index(self.current_style))
        style_combo.grid(column=1, row=1)
        style_combo.bind("<<ComboboxSelected>>", set_style)

        # UID format
        uid_label = tkinter.ttk.Label(window, width=15, text='Format UID:', anchor='w')
        uid_label.grid(row=2, column=0, padx=5, pady=5)
        uid_entry = tkinter.ttk.Entry(window, width=30)
        uid_entry.insert(tkinter.END, self.uid_fmt)
        uid_entry.grid(row=2, column=1, padx=5, pady=5)
        
        # Save button
        def save():
            self.uid_fmt = uid_entry.get()
            self.check_db()
            self.update_view()
            root.destroy()
        bottom_row = tkinter.ttk.Frame(window)
        bottom_row.grid(row=3, column=0, columnspan=2)
        button_save = tkinter.ttk.Button(bottom_row, text='Save!', command=save)
        button_save.grid(row=0, column=0, padx=5, pady=5)

    def helpme(self):
        def open_file(filename):
            if sys.platform == "win32":
                os.startfile(filename)
            else:
                opener = "open" if sys.platform == "darwin" else "xdg-open"
                subprocess.call([opener, filename])
        paths = [ os.path.abspath(os.path.dirname(__file__)) ]
        for p in paths:
            f = os.path.join(p, 'manual.pdf')
            if os.path.isfile(f):
                open_file(f)
                return

    def update_view(self):
        self.listMinerals.delete(0, tkinter.END)
        if self.sort_by=='Number':
            sort_minerals = sorted(self.minerals.values(), key=lambda mineral: mineral['Number'])
        else:
            sort_minerals = sorted(self.minerals.values(), key=lambda mineral: mineral['UID'])
        for mineral in sort_minerals:
            self.listMinerals.insert(tkinter.END, mineral['UID'])

    def read_from_file(self):
        fname = tkinter.filedialog.askopenfilename(title="Select file")
        if fname:
            if not os.path.isfile(fname):
                print("ERROR! <%s> does not exist. Doing nothing." % (fname))
                return
            with open(fname, 'r') as fp:
                self.minerals = json.load(fp)
            if not self.image_path:
                self.image_path = os.path.join(os.path.dirname(fname), 'images')
                if not os.path.isdir(self.image_path):
                    self.image_path = None
        self.check_db()
        self.update_view()

    def get_new_number(self):
        return max([0]+[int(m['Number']) for m in self.minerals.values()])+1

    def get_id(self, m):
        # %S -> Species
        # %C -> Class
        # %N -> Number
        fmt = self.uid_fmt
        specie = re.sub('[^a-zA-Z0-9 \n\.]', '', m['Species'])
        klass = m['Class'].split(',')[0]
        number = '%d' % int(m['Number'])
        params = list()
        l = len(fmt)
        for i, c in enumerate(fmt):
            if c=='%' and i+1<=l:
                k = fmt[i+1]
                if k == 'S':
                    params.append(specie)
                elif k == 'C':
                    params.append(klass)
                elif k == 'N':
                    params.append(number)
        fmt = fmt.replace('%S', '%s')
        fmt = fmt.replace('%C', '%s')
        fmt = fmt.replace('%N', '%s')
        uid = fmt % (*params,)
        uid.replace(" ", '-')
        return uid

    def check_db(self):
        checked = {}
        for uid, mineral in self.minerals.items():
            for key in self.fields:
                if not key in mineral:
                    mineral[key] = None
                if mineral[key]=="":
                    mineral[key] = None
            for key in mineral.keys():
                if key not in self.fields:
                    print("WARNING! Key %s is not recognised!" % (key))
            new_uid = self.get_id(mineral)
            if new_uid!=uid:
                mineral['UID'] = new_uid
            checked[new_uid] = mineral
        self.minerals = checked

    def save_to_file(self):
        fname = tkinter.filedialog.asksaveasfilename(initialdir=os.getcwd(), title="Select file")
        if fname:
            with open(fname, 'w') as fp:
                json.dump(self.minerals, fp, indent=4, sort_keys=True)

    def load_images(self, uid):
        self.images = {}
        if not self.image_path:
            return
        file_list = os.listdir(self.image_path)
        for fname in file_list:
            if fname.startswith(uid):
                image_file = PIL.Image.open(os.path.join(self.image_path, fname))
                width, height = image_file.size
                scale_factor = self.max_fig_size/max(width, height)
                image_scaled = image_file.resize((int(width*scale_factor), int(height*scale_factor)), PIL.Image.ANTIALIAS)
                image_render = PIL.ImageTk.PhotoImage(image_scaled)
                desc = fname.replace(uid, '')
                ext = desc.split('.')[-1]
                desc = desc.replace('.'+ext, '')
                if desc=='':
                    desc = 'reference'
                else:
                    desc = desc[1:]
                self.images[desc] = image_render

    def insert_chemical_formula(self, text, formula):
        text.insert(tkinter.END, '%-16s : ' % ('Chemical Formula'))
        if not formula:
            text.insert(tkinter.END, 'None\n')
            return
        subscript = True
        for char in formula:
            if char.isdigit() and subscript:
                text.insert(tkinter.END, char, 'subscript')
            elif char=='.':
                text.insert(tkinter.END, '\u30fb')
                subscript = False
            else:
                text.insert(tkinter.END, char)
                if char.isalpha():
                    subscript = True
        text.insert(tkinter.END, '\n')

    def show_minerals(self):
        def onselect(evt):
            w = evt.widget
            select = w.curselection()
            if len(select)>0:
                index = int(select[0])
            else:
                return
            value = w.get(index)
            mineral = self.minerals[value]
            text.config(state='normal')
            text.delete(1.0, tkinter.END)
            text.tag_configure("center", justify='center')
            text.tag_configure("superscript", offset=4)
            text.tag_configure("subscript", offset=-4)
            text.insert(tkinter.END, mineral['Species'] + '\n')
            text.insert(tkinter.END, '-'*len(mineral['Species']) + '\n')
            for field in self.fields:
                if field=='Chemical Formula':
                    self.insert_chemical_formula(text, mineral['Chemical Formula'])
                #elif field=='Class':
                #    text.insert(tkinter.END, 'Class:\t%s\n' % (mineral['Class']))
                #    for key,val in self.nickel_strunz.items():
                #        if mineral['Class'].startswith(key):
                #            text.insert(tkinter.END, '\t%s: %s\n' % (key, val))
                else:
                    text.insert(tkinter.END, '%-16s : %s\n' % (field, str(mineral[field])))
            text.insert(tkinter.END, '\n')
            self.load_images(mineral['UID'])
            if 'reference' in self.images.keys():
                text.image_create(tkinter.END, image=self.images['reference'])
                text.tag_add('center', 'current linestart', 'current lineend')
                text.insert(tkinter.END, '\n\n')
            for desc, img in self.images.items():
                if desc is not 'reference':
                    if desc=='UVSW':
                        text.insert(tkinter.END, 'Short-Wave UV\n')
                    elif desc=='UVLW':
                        text.insert(tkinter.END, 'Long-Wave UV\n')
                    else:
                        text.insert(tkinter.END, desc+'\n')
                    text.image_create(tkinter.END, image=img)
                    text.tag_add('center', 'current linestart', 'current lineend')
                    text.insert(tkinter.END, '\n\n')
            text.config(state='disabled')
            text.bind("<1>", lambda event: text.focus_set())
            self.selected = value

        window = tkinter.ttk.Frame(self.parent)
        window.grid(row=1, column=0, sticky='nesw', padx=0, pady=0)
        window.rowconfigure(0, weight=1)
        window.columnconfigure(0, weight=1)
        window.columnconfigure(1, weight=2)

        frm1 = tkinter.ttk.Frame(window, borderwidth=2, relief='groove')
        frm1.grid(row=0, column=0, sticky='nesw', padx=5, pady=5)
        frm1.columnconfigure(0, weight=1)
        frm1.rowconfigure(0, weight=1)
        scroll1y = AutoScrollbar(frm1, orient="vertical")
        scroll1y.grid(row=0, column=1, sticky='ns')
        scroll1x = AutoScrollbar(frm1, orient="horizontal")
        scroll1x.grid(row=1, column=0, sticky='ew')
        self.listMinerals = tkinter.Listbox(frm1, yscrollcommand=scroll1y.set, xscrollcommand=scroll1x.set)
        self.listMinerals.grid(row=0, column=0, sticky='nesw', padx=5, pady=5)
        self.listMinerals.bind('<<ListboxSelect>>', onselect)
        scroll1y.config(command=self.listMinerals.yview)
        scroll1x.config(command=self.listMinerals.xview)
        self.listMinerals.config(borderwidth=0)

        frm2 = tkinter.ttk.Frame(window, borderwidth=2, relief='groove')
        frm2.grid(row=0, column=1, sticky='nesw', padx=5, pady=5)
        frm2.columnconfigure(0, weight=1)
        frm2.rowconfigure(0, weight=1)
        scroll2y = AutoScrollbar(frm2, orient="vertical")
        scroll2y.grid(row=0, column=1, sticky='ns')
        scroll2x = AutoScrollbar(frm2, orient="horizontal")
        scroll2x.grid(row=1, column=0, sticky='ew')
        text = tkinter.Text(frm2, wrap='word', state='disabled', highlightthickness=0, yscrollcommand=scroll2y.set, xscrollcommand=scroll2x.set)
        text.grid(row=0, column=0, sticky='nesw', padx=5, pady=5)
        scroll2y.config(command=text.yview)
        scroll2x.config(command=text.xview)

    def add_to_db(self, data, old_uid=None):
        mineral = {}
        idcode = None
        if old_uid:
            idcode = self.minerals[old_uid]['Number']
            self.minerals.pop(old_uid)
        for field in self.fields:
            value = None
            if field in data:
                tmp = data[field].strip()
                if tmp:
                    value = tmp
            mineral[field] = value
        if not mineral['Species']:
            tkinter.messagebox.showerror("ERROR!", "You did not specify the mineral name! This is required!")
            return
        if not mineral['Class']:
            mineral['Class'] = 'X.XX.XX'
        if idcode:
            mineral['Number'] = int(idcode)
        if mineral['Number']:
            used_numbers = [ int(m['Number']) for m in self.minerals.values() ]
            if mineral['Number'] in used_numbers:
                print('Error! Number %d already used! Generating a new one...' % mineral['Number'])
                mineral['Number'] = self.get_new_number()
        else:
            mineral['Number'] = self.get_new_number()
        
        mineral_id = self.get_id(mineral)
        mineral['UID'] = mineral_id
        self.minerals[mineral_id] = mineral
        self.update_view()

    def addmod_mineral(self, modify=False):
        def fetch(entries):
            for entry in entries:
                field = entry[0]
                text  = entry[1].get()
                mineral[field] = text
            self.add_to_db(mineral, old_uid=old_uid)
            root.destroy()
        def makeform(root):
            entries = []
            for row, field in enumerate(self.fields):
                lab = tkinter.ttk.Label(window, width=15, text=field, anchor='w')
                lab.grid(row=row, column=0, padx=5, pady=5)
                ent = tkinter.ttk.Entry(window, width=30)
                ent.grid(row=row, column=1, padx=5, pady=5, sticky='ew')
                if modify and mineral[field]:
                    ent.insert(tkinter.END, mineral[field])
                entries.append((field, ent))
            return entries
        if modify:
            if self.selected:
                mineral = self.minerals[self.selected]
                old_uid = mineral['UID']
            else:
                return
        else:
            mineral = {}
            old_uid = None
        root = tkinter.Toplevel(self.parent)
        if modify:
            root.title('Modify entry')
        else:
            root.title('Add a new mineral')
        root.columnconfigure(0, weight=1)
        root.rowconfigure(0, weight=1)
        window = tkinter.ttk.Frame(root)
        window.grid(row=0, column=0, sticky='news')
        window.columnconfigure(1, weight=1)
        ents = makeform(root)
        bottom_row = tkinter.ttk.Frame(window)
        bottom_row.grid(row=len(self.fields), column=0, columnspan=2)
        if modify:
            b1 = tkinter.ttk.Button(bottom_row, text='Save!', command=(lambda e=ents: fetch(e)))
        else:
            b1 = tkinter.ttk.Button(bottom_row, text='Add Mineral!', command=(lambda e=ents: fetch(e)))
        b1.grid(row=0, column=0, padx=5, pady=5)
        b2 = tkinter.ttk.Button(bottom_row, text='Quit', command=root.destroy)
        b2.grid(row=0, column=1, padx=5, pady=5)

    def add_mineral(self):
        self.addmod_mineral(modify=False)

    def modify_selected(self):
        self.addmod_mineral(modify=True)

    #def read_nickel_strunz(self):
    #    ns = dict()
        #with open('nickel-strunz.txt', 'r') as fp:
        #    for line in fp.readlines():
        #        if not line or line[0]=='#' or len(line)==1:
        #            continue
        #        code, desc = line.strip().split(' ', 1)
        #        ns[code] = desc
    #    self.nickel_strunz = ns

    def export_report(self):
        fname = tkinter.filedialog.asksaveasfilename(initialdir=os.getcwd(), title="Select file")
        if fname:
            with open(fname, 'w') as fp:
                fp.write("\\documentclass[10pt,letterpaper]{article}\n")
                fp.write("\\usepackage[utf8]{inputenc}\n")
                fp.write("\\usepackage{amsmath}\n")
                fp.write("\\usepackage{amsfonts}\n")
                fp.write("\\usepackage{amssymb}\n")
                fp.write("\\usepackage{graphicx}\n")
                fp.write("\\usepackage{tabularx}\n")
                fp.write("\\usepackage[version=4]{mhchem}\n")
                fp.write("\\usepackage[paperwidth=5.5in, paperheight=8.5in,left=2cm,right=2cm,top=2cm,bottom=2cm]{geometry}\n\n")
                fp.write("\\begin{document}\n\n")
                for minkey in sorted(self.minerals.keys()):
                    mineral = self.minerals[minkey]
                    fp.write("\\section*{SC%s - %s}\n" % (str(mineral['Number']), mineral['Species']))
                    fp.write("\\begin{tabularx}{\\textwidth}{l>{\\raggedright\\arraybackslash}X}\n")
                    for key in self.fields:
                        text = str(mineral[key])
                        text = text.replace("_", "\_")
                        text = text.replace("%", "\%")
                        text = text.replace("#", "\#")
                        text = text.replace("$", "\$")
                        if key=="Chemical Formula":
                            fp.write("\\textbf{%s:} & \\ce{%s}\\\\\n" % (key, text))
                        elif key=='Number':
                            fp.write("\\textbf{%s:} & SC%s\\\\\n" % (key, text))
                        elif key=='Class':
                            fp.write("\\textbf{%s:} & %s\\\\\n" % (key, text))
                            #for nskey,nsval in self.nickel_strunz.items():
                            #    if mineral['Class'].startswith(nskey):
                            #        fp.write(" & %s: %s\\\\\n" % (nskey, nsval))
                        else:
                            fp.write("\\textbf{%s:} & %s\\\\\n" % (key, text))
                    fp.write("\\end{tabularx}\n")
                    fp.write("\\clearpage\n\n")
                fp.write("\\end{document}")

def main_gui():
    root = tkinter.Tk()
    MineralApp(root)
    root.mainloop()

if __name__ == "__main__":
    main_gui()


