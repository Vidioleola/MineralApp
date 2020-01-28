#!/usr/bin/env python3

import os, sys, io, json, re
import subprocess
import tkinter
import tkinter.ttk
import tkinter.filedialog
import tkinter.messagebox
import PIL, PIL.ImageTk
import ttkthemes
import webbrowser
import functools
import reportlab, reportlab.platypus, reportlab.lib.styles
import urllib.request
import sqlite3

try:
    from .version import version
except ImportError:
    from version import version

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


class HyperlinkManager:

    def __init__(self, text):
        self.text = text
        self.text.tag_config("hyper", foreground="blue", underline=1)
        self.text.tag_bind("hyper", "<Enter>", self._enter)
        self.text.tag_bind("hyper", "<Leave>", self._leave)
        self.text.tag_bind("hyper", "<Button-1>", self._click)
        self.reset()

    def reset(self):
        self.links = {}

    def add(self, action):
        tag = "hyper-%d" % len(self.links)
        self.links[tag] = action
        return "hyper", tag

    def _enter(self, event):
        self.text.config(cursor="hand2")

    def _leave(self, event):
        self.text.config(cursor="")

    def _click(self, event):
        for tag in self.text.tag_names(tkinter.CURRENT):
            if tag[:6] == "hyper-":
                self.links[tag]()
                return

def click(url):
    if sys.platform=='darwin':
        subprocess.call(['open', url])
    else:
        webbrowser.open(url)


class MineralApp(tkinter.ttk.Frame):

    fields = [ 'Name', 'Number', 'UID', 'Locality', 'Acquisition', 'Size', 'Weight', 'Price', 'Collection',
        'Species', 'Class', 'Chemical Formula', 'Color', 'Fluorescence (SW)', 'Fluorescence (MW)',
        'Fluorescence (LW)', 'Fluorescence (405nm)', 'Phosphorescence', 'Tenebrescence',
        'Radioactivity', 'Comments' ]
    minerals = dict()
    selected = None
    settings = {
        'uid_fmt'       : '%C_%S_%N',
        'sort_by'       : 'Class',
        'current_style' : 'plastik',
        'max_fig_size'  : 400.0,
        'image_path'    : None,
        'check_version' : False,
        'version'       : version
        }

    def __init__(self, parent, *args, **kwargs):
        tkinter.ttk.Frame.__init__(self, parent, *args, **kwargs)
        self.winfo_toplevel().title("MineralApp " + version)
        self.set_icon()
        self.style = ttkthemes.ThemedStyle(self)
        self.style.theme_use(self.settings['current_style'])
        self.parent = parent
        self.parent.rowconfigure(0, weight=0)
        self.parent.rowconfigure(1, weight=1)
        self.parent.columnconfigure(0, weight=1)
        self.top_panel = tkinter.ttk.Frame(self.parent)
        self.top_panel.grid(row=0, column=0, padx=0, pady=0, sticky='news')
        self.top_panel.columnconfigure(0, weight=1)
        self.top_panel.columnconfigure(9, weight=1)
        self.button_load = tkinter.ttk.Button(self.top_panel, text="Load database", command=self.read_from_file).grid(row=0, column=1, padx=5, pady=5)
        self.button_save = tkinter.ttk.Button(self.top_panel, text="Save database", command=self.save_to_file).grid(row=0, column=2, padx=5, pady=5)
        self.button_add_mineral = tkinter.ttk.Button(self.top_panel, text="Add new mineral", command=self.add_mineral).grid(row=0, column=3, padx=5, pady=5)
        self.button_modify = tkinter.ttk.Button(self.top_panel, text="Modify selected", command=self.modify_selected).grid(row=0, column=4, padx=5, pady=5)
        self.button_modify = tkinter.ttk.Button(self.top_panel, text="Export", command=self.export_report).grid(row=0, column=5, padx=5, pady=5)
        self.button_modify = tkinter.ttk.Button(self.top_panel, text="Export v2", command=self.export_v2).grid(row=0, column=6, padx=5, pady=5)
        self.button_settings = tkinter.ttk.Button(self.top_panel, text="Settings", command=self.manage_settings).grid(row=0, column=7, padx=5, pady=5)
        self.button_help = tkinter.ttk.Button(self.top_panel, text="Help me!", command=self.helpme).grid(row=0, column=8, padx=5, pady=5)
        self.button_quit = tkinter.ttk.Button(self.top_panel, text="Quit", command=self.parent.destroy).grid(row=0, column=9, padx=5, pady=5)
        self.show_minerals()
        self.update_view()
        self.check_new_version()

    def check_new_version(self):
        if not self.settings['check_version']:
            return
        try:
            fp = urllib.request.urlopen("https://raw.githubusercontent.com/SimoneCnt/MineralApp/master/mineralapp/version.py")
            retcode = fp.getcode()
            if retcode!=200:
                return
            data = fp.read().decode()
            newv = data.split('=')[1].replace('"','')
            if float(newv)>float(version):
                tkinter.messagebox.showinfo("Update available!", "You are using version %s, but version %s is available!" % (version, newv))
        except:
            pass

    def set_icon(self):
        dirs = [ os.getcwd(), os.path.dirname(__file__), '.', '..', 'icon', '../icon/' ]
        for d in dirs:
            p = os.path.join(d, 'icon-512.png')
            if os.path.isfile(p):
                img = PIL.ImageTk.PhotoImage(PIL.Image.open(p))
                self.winfo_toplevel().tk.call('wm', 'iconphoto', self.winfo_toplevel()._w, img)
                return

    def manage_settings(self):
        root = tkinter.Toplevel(self.parent)
        root.title('Settings')
        root.columnconfigure(0, weight=1)
        root.rowconfigure(0, weight=1)
        window = tkinter.ttk.Frame(root)
        window.grid(row=0, column=0, sticky='news')
        window.columnconfigure(1, weight=1)

        # Sort option
        def set_sort(event):
            self.settings['sort_by'] = sort_combo.get()
            self.update_view()
        sort_label = tkinter.ttk.Label(window, width=15, text='Sort by:', anchor='w')
        sort_label.grid(row=0, column=0, padx=5, pady=5)
        sort_values = ['Class', 'Number']
        sort_combo = tkinter.ttk.Combobox(window)
        sort_combo['values'] = sort_values
        sort_combo.current(sort_values.index(self.settings['sort_by']))
        sort_combo.grid(column=1, row=0)
        sort_combo.bind("<<ComboboxSelected>>", set_sort)

        # Windows style
        def set_style(event):
            self.settings['current_style'] = style_combo.get()
            self.style.theme_use(self.settings['current_style'])
        style_label = tkinter.ttk.Label(window, width=15, text='Style:', anchor='w')
        style_label.grid(row=1, column=0, padx=5, pady=5)
        style_combo = tkinter.ttk.Combobox(window)
        style_combo['values'] = self.style.theme_names()
        style_combo.current(self.style.theme_names().index(self.settings['current_style']))
        style_combo.grid(column=1, row=1)
        style_combo.bind("<<ComboboxSelected>>", set_style)

        # UID format
        uid_label = tkinter.ttk.Label(window, width=15, text='Format UID:', anchor='w')
        uid_label.grid(row=2, column=0, padx=5, pady=5)
        uid_entry = tkinter.ttk.Entry(window, width=30)
        uid_entry.insert(tkinter.END, self.settings['uid_fmt'])
        uid_entry.grid(row=2, column=1, padx=5, pady=5)
        
        # Save button
        def save():
            self.settings['uid_fmt'] = uid_entry.get()
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
        if self.settings['sort_by']=='Number':
            sort_minerals = sorted(self.minerals.values(), key=lambda mineral: mineral['Number'])
        else:
            sort_minerals = sorted(self.minerals.values(), key=lambda mineral: mineral['UID'])
        for mineral in sort_minerals:
            self.listMinerals.insert(tkinter.END, mineral['UID'])

    def read_from_file(self):
        fname = tkinter.filedialog.askopenfilename(title="Select file")
        if fname:
            if not os.path.isfile(fname):
                tkinter.messagebox.showerror("ERROR!", "<%s> does not exist. Doing nothing." % (fname))
                return
            with open(fname, 'r') as fp:
                data = json.load(fp)
                if 'settings' not in data.keys():
                    self.minerals = data
                else:
                    self.minerals = data['minerals']
                    self.settings.update(data['settings'])
            if not self.settings['image_path']:
                self.settings['image_path'] = os.path.join(os.path.dirname(fname), 'images')
                if not os.path.isdir(self.settings['image_path']):
                    self.settings['image_path'] = None
            self.style.theme_use(self.settings['current_style'])
            self.check_new_version()
            self.check_db()
            self.update_view()

    def get_new_number(self):
        return max([0]+[int(m['Number']) for m in self.minerals.values()])+1

    def get_id(self, m):
        # %S -> Species
        # %C -> Class
        # %N -> Number
        fmt = self.settings['uid_fmt']
        name = re.sub('[^a-zA-Z0-9 ]', '', m['Name']).strip()
        klass = m['Class'].split(';;')[0].strip()
        number = '%d' % int(m['Number'])
        params = list()
        l = len(fmt)
        for i, c in enumerate(fmt):
            if c=='%' and i+1<=l:
                k = fmt[i+1]
                if k == 'S':
                    params.append(name)
                elif k == 'C':
                    params.append(klass)
                elif k == 'N':
                    params.append(number)
        fmt = fmt.replace('%S', '%s')
        fmt = fmt.replace('%C', '%s')
        fmt = fmt.replace('%N', '%s')
        uid = fmt % (*params,)
        uid = uid.replace(" ", '-')
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
                    tkinter.messagebox.showerror("WARNING!", "Key %s is not recognised!" % (key))
            new_uid = self.get_id(mineral)
            if new_uid!=uid:
                mineral['UID'] = new_uid
            checked[new_uid] = mineral
        self.minerals = checked

    def save_to_file(self):
        fname = tkinter.filedialog.asksaveasfilename(title="Select file", initialfile='minerals.json')
        if fname:
            tosave = {'settings': self.settings, 'minerals': self.minerals}
            with open(fname, 'w') as fp:
                json.dump(tosave, fp, indent=4, sort_keys=True)

    def load_images(self, uid):
        self.images = {}
        if not self.settings['image_path']:
            return
        if not os.path.isdir(self.settings['image_path']):
            return
        minid = str(self.minerals[uid]['Number'])
        basedir = os.path.join(self.settings['image_path'], minid)
        if not os.path.isdir(basedir):
            return
        file_list = os.listdir(basedir)
        for fname in file_list:
            basename, extension = os.path.splitext(fname)
            image_uid = '_'.join(basename.split('_')[0:3])
            if image_uid==uid:
                image_file = PIL.Image.open(os.path.join(basedir, fname))
                width, height = image_file.size
                scale_factor = self.settings['max_fig_size']/max(width, height)
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
        if not formula:
            return
        formula = formula.replace(' . ', '\u30fb')
        subscript = True
        for n,char in enumerate(formula):
            if n<len(formula)-1:
                nchar = formula[n+1]
            else:
                nchar = None
            if char in ['+', '-'] or nchar in ['+', '-']:
                text.insert(tkinter.END, char, 'superscript')
            elif (char.isdigit() or char=='.') and subscript:
                text.insert(tkinter.END, char, 'subscript')
            elif char=='\u30fb':
                text.insert(tkinter.END, '\u30fb')
                subscript = False
            else:
                text.insert(tkinter.END, char)
                if char.isalpha():
                    subscript = True

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

            # Mineral Name
            text.insert(tkinter.END, mineral['Name'] + '\n')
            text.insert(tkinter.END, '-'*len(mineral['Name']) + '\n')

            # Spacer
            text.insert(tkinter.END, '\n')

            # Number of UID
            text.insert(tkinter.END, '%-20s : %s\n' % ('Number', str(mineral['Number'])))
            text.insert(tkinter.END, '%-20s : %s\n' % ('UID', str(mineral['UID'])))

            # Spacer
            text.insert(tkinter.END, '\n')

            # Regex to find [[MINDAT:xxx-####]]
            regex = re.compile('\[\[MINDAT:(\w{3}-\d*)\]\]')

            # Locality
            text.insert(tkinter.END, '%-20s : ' % ('Locality'))
            locality = mineral['Locality']
            if locality:
                results = regex.findall(locality)
                for res in results:
                    locality = locality.replace('[[MINDAT:%s]]' % (res), '')
                locality = locality.strip()
                text.insert(tkinter.END, '%s' % (locality))
                for res in results:
                    text.insert(tkinter.END, " ")
                    text.insert(tkinter.END, "(mindat)", hyperlink.add(functools.partial(click, 'http://www.mindat.org/%s.html' % (res))))
                text.insert(tkinter.END, '\n')
            else:
                text.insert(tkinter.END, 'Null\n')

            # Overall info of sample
            text.insert(tkinter.END, '%-20s : %s\n' % ('Acquisition', str(mineral['Acquisition'])))
            text.insert(tkinter.END, '%-20s : %s\n' % ('Size', str(mineral['Size'])))
            text.insert(tkinter.END, '%-20s : %s\n' % ('Weight', str(mineral['Weight'])))
            text.insert(tkinter.END, '%-20s : %s\n' % ('Price/Value', str(mineral['Price'])))
            text.insert(tkinter.END, '%-20s : %s\n' % ('Collection', str(mineral['Collection'])))

            # Spacer
            text.insert(tkinter.END, '\n')

            # Info of each species
            keys = ['Species', 'Chemical Formula', 'Class', 'Color', 'Fluorescence (SW)', 'Fluorescence (MW)',
                'Fluorescence (LW)', 'Fluorescence (405nm)', 'Phosphorescence', 'Tenebrescence', 'Radioactivity']
            for key in keys:
                if mineral[key]:
                    vals = mineral[key].split(';;')
                    vv = [ v for v in vals if v.lower().strip()!='no']
                    if not any(vv):
                        continue
                    text.insert(tkinter.END, '%-20s : ' % (key))
                    for v in vals:
                        v = v.strip()
                        if key=='Chemical Formula':
                            self.insert_chemical_formula(text, v)
                            text.insert(tkinter.END, ' '*(20-len(v)))
                        elif key=='Species':
                            text.insert(tkinter.END, '%s' % (v), hyperlink.add(functools.partial(click, 'http://www.mindat.org/show.php?name=%s' % (v))))
                            text.insert(tkinter.END, ' '*(20-len(v)))
                        else:
                            text.insert(tkinter.END, '%-20s' % (v))
                    text.insert(tkinter.END, '\n')

            # Comments
            if mineral['Comments']:
                text.insert(tkinter.END, '\n')
                text.insert(tkinter.END, '%-20s : %s\n' % ('Comments', str(mineral['Comments'])))

            # Images
            text.insert(tkinter.END, '\n')
            self.load_images(mineral['UID'])
            if 'reference' in self.images.keys():
                text.image_create(tkinter.END, image=self.images['reference'])
                text.tag_add('center', 'current linestart', 'current lineend')
                text.insert(tkinter.END, '\n\n')
            for desc, img in self.images.items():
                if desc!='reference':
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
        hyperlink = HyperlinkManager(text)
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
        if not mineral['Name']:
            tkinter.messagebox.showerror("ERROR!", "You did not specify the mineral name! This is required!")
            return
        if not mineral['Class']:
            mineral['Class'] = 'X.XX.XX'
        if idcode:
            mineral['Number'] = int(idcode)
        if mineral['Number']:
            mineral['Number'] = int(mineral['Number'])
            used_numbers = [ int(m['Number']) for m in self.minerals.values() ]
            if mineral['Number'] in used_numbers:
                tkinter.messagebox.showerror('Error!', 'Number %d already used! Generating a new one...' % mineral['Number'])
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
                lab = tkinter.ttk.Label(window, width=20, text=field, anchor='w')
                lab.grid(row=row, column=0, padx=5, pady=5)
                ent = tkinter.ttk.Entry(window, width=100)
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

    def _report_write_data(self, mineral):
        story = list()
        styles = reportlab.lib.styles.getSampleStyleSheet()
        normal = styles['Normal']
        normal.fontSize = 10
        title = styles["Heading1"]
        title.fontSize = 14
        # Title
        ptext = '%s [%d]' % (mineral['Name'], mineral['Number'])
        story.append(reportlab.platypus.Paragraph(ptext, title))
        # Sample properties
        fields = [ 'UID', 'Locality', 'Acquisition', 'Size', 'Weight', 'Price']
        data = list()
        for field in fields:
            p1 = reportlab.platypus.Paragraph('<b>%s</b>:' % (field), normal)
            p2 = reportlab.platypus.Paragraph(str(mineral[field]), normal)
            data.append([p1, p2])
        t = reportlab.platypus.Table(data, colWidths=[4*reportlab.lib.units.cm, None])
        t.setStyle(reportlab.platypus.TableStyle([
                ('VALIGN', (0, 0), (-1, -1), 'TOP'),
                ('LEFTPADDING',(0,0),(-1,-1), 0),
                ('RIGHTPADDING',(0,0),(-1,-1), 0),
            ]))
        t.hAlign = 'LEFT'
        story.append(t)
        # Species properties
        fields = ['Species', 'Class', 'Chemical Formula', 'Color', 'Fluorescence (SW)', 'Fluorescence (MW)',
            'Fluorescence (LW)', 'Fluorescence (405nm)', 'Phosphorescence', 'Tenebrescence', 'Radioactivity']
        data = list()
        for field in fields:
            if not mineral[field] or mineral[field].lower()=='no':
                continue
            vv = [ v for v in str(mineral[field]).split(';;') if v.lower().strip()!='no']
            if not any(vv):
                continue
            vals = [reportlab.platypus.Paragraph('<b>'+field+':</b>', normal)] + [reportlab.platypus.Paragraph(s, normal) for s in str(mineral[field]).split(';;') ]
            data.append(vals)
        ns = len(mineral['Species'].split(';;'))
        t = reportlab.platypus.Table(data, colWidths=[4*reportlab.lib.units.cm]+[None]*ns)
        t.setStyle(reportlab.platypus.TableStyle([
                ('VALIGN', (0, 0), (-1, -1), 'TOP'),
                ('LEFTPADDING',(0,0),(-1,-1), 0),
                ('RIGHTPADDING',(0,0),(-1,-1), 0),
            ]))
        t.hAlign = 'LEFT'
        story.append(t)
        # Comments are last
        field = 'Comments'
        data = list()
        p1 = reportlab.platypus.Paragraph('<b>%s</b>:' % (field), normal)
        p2 = reportlab.platypus.Paragraph(str(mineral[field]), normal)
        data.append([p1, p2])
        t = reportlab.platypus.Table(data, colWidths=[4*reportlab.lib.units.cm, None])
        t.setStyle(reportlab.platypus.TableStyle([
                ('VALIGN', (0, 0), (-1, -1), 'TOP'),
                ('LEFTPADDING',(0,0),(-1,-1), 0),
                ('RIGHTPADDING',(0,0),(-1,-1), 0),
            ]))
        t.hAlign = 'LEFT'
        story.append(t)
        story.append(reportlab.platypus.Spacer(1, 12))
        return story

    def _report_insert_images(self, uid, max_fig_size=5*reportlab.lib.units.cm, scale_quality=2.0):
        if not self.settings['image_path']:
            return list()
        story = list()
        file_list = os.listdir(self.settings['image_path'])
        images = list()
        for fname in file_list:
            basename, extension = os.path.splitext(fname)
            image_uid = '_'.join(basename.split('_')[0:3])
            if image_uid==uid:
                # Create image with PIL and resize it
                image_file = PIL.Image.open(os.path.join(self.settings['image_path'], fname))
                width, height = image_file.size
                scale_factor = scale_quality*max_fig_size/max(width, height)
                image_scaled = image_file.resize((int(width*scale_factor), int(height*scale_factor)), PIL.Image.ANTIALIAS)
                # Save to bytestring
                image_str = io.BytesIO()
                image_scaled.save(image_str, 'PNG')
                image_str.seek(0)
                # Create ReportLab Image from bytestring
                image = reportlab.platypus.Image(image_str)
                image.drawHeight /= scale_quality
                image.drawWidth /= scale_quality
                images.append(image)
        if len(images)==0:
            return list()
        data = list()
        row = list()
        for i, img in enumerate(images):
            if i%3==0 and len(row)>0:
                data.append(row)
                row = list()
            row.append(img)
        data.append(row)
        t = reportlab.platypus.Table(data, colWidths=None)
        t.setStyle(reportlab.platypus.TableStyle([
                ('VALIGN', (0, 0), (-1, -1), 'MIDDLE'),
                ('ALIGN', (0, 0), (-1, -1), 'CENTER'),
            ]))
        story.append(t)
        story.append(reportlab.platypus.Spacer(1, 12))
        return story


    def export_report(self):
        fname = tkinter.filedialog.asksaveasfilename(title="Select file", initialfile='report.pdf')
        if fname:
            doc = reportlab.platypus.SimpleDocTemplate(fname, pagesize=reportlab.lib.pagesizes.letter,
                rightMargin=72, leftMargin=72, topMargin=72, bottomMargin=18,
                title='Mineral Collection Report', author='MineralApp - https://github.com/SimoneCnt/MineralApp')
            story = list()
            for mineral in self.minerals.values():
                story += self._report_write_data(mineral)
                story += self._report_insert_images(mineral['UID'])
            doc.build(story)

    def export_v2(self):
        fname = tkinter.filedialog.asksaveasfilename(title="Select file", initialfile='minerals.sqlite3')
        if not fname:
            return
        if os.path.isfile(fname):
            os.remove(fname)
        conn = sqlite3.connect(fname)
        c = conn.cursor()
        c.execute("CREATE TABLE MINERALS (MINID INTEGER PRIMARY KEY, NAME TEXT NOT NULL, LOCALITY TEXT, LOCID_MNDAT TEXT, SIZE TEXT, WEIGHT TEXT, ACQUISITION TEXT, COLLECTION TEXT, VALUE TEXT, S1_SPECIES TEXT, S1_CLASS TEXT, S1_CHEMF TEXT, S1_COLOR TEXT, S1_FLSW TEXT, S1_FLMW TEXT, S1_FLLW TEXT, S1_FL405 TEXT, S1_PHSW TEXT, S1_PHMW TEXT, S1_PHLW TEXT, S1_PH405 TEXT, S1_TENEBR TEXT, S2_SPECIES TEXT, S2_CLASS TEXT, S2_CHEMF TEXT, S2_COLOR TEXT, S2_FLSW TEXT, S2_FLMW TEXT, S2_FLLW TEXT, S2_FL405 TEXT, S2_PHSW TEXT, S2_PHMW TEXT, S2_PHLW TEXT, S2_PH405 TEXT, S2_TENEBR TEXT, S3_SPECIES TEXT, S3_CLASS TEXT, S3_CHEMF TEXT, S3_COLOR TEXT, S3_FLSW TEXT, S3_FLMW TEXT, S3_FLLW TEXT, S3_FL405 TEXT, S3_PHSW TEXT, S3_PHMW TEXT, S3_PHLW TEXT, S3_PH405 TEXT, S3_TENEBR TEXT, S4_SPECIES TEXT, S4_CLASS TEXT, S4_CHEMF TEXT, S4_COLOR TEXT, S4_FLSW TEXT, S4_FLMW TEXT, S4_FLLW TEXT, S4_FL405 TEXT, S4_PHSW TEXT, S4_PHMW TEXT, S4_PHLW TEXT, S4_PH405 TEXT, S4_TENEBR TEXT, RADIOACT TEXT, COMMENTS TEXT );")
        c.execute("CREATE TABLE SETTINGS (VERSION_MAJOR INT, VERSION_MINOR INT)")
        c.execute("INSERT INTO SETTINGS (VERSION_MAJOR, VERSION_MINOR) VALUES (2, 0);")
        mindata = list()
        regex = re.compile('\[\[MINDAT:(\w{3}-\d*)\]\]')
        for mine in self.minerals.values():
            locality = mine['Locality']
            locid = ''
            if locality:
                for res in regex.findall(locality):
                    locality = locality.replace('[[MINDAT:%s]]' % (res), '')
                    locid = res
                locality = locality.strip()
            size = mine['Size']
            data = (int(mine['Number']), mine['Name'], locality, locid, mine['Size'], mine['Weight'], mine['Acquisition'], mine['Collection'], mine['Price'])
            fields = [ 'Species', 'Class', 'Chemical Formula', 'Color', 'Fluorescence (SW)', 'Fluorescence (MW)',
                        'Fluorescence (LW)', 'Fluorescence (405nm)', 'Phosphorescence', 'Phosphorescence', 'Phosphorescence', 'Phosphorescence', 'Tenebrescence' ]
            for i in range(4):
                for f in fields:
                    toadd = ''
                    if mine[f]:
                        dd = [d.strip() for d in mine[f].split(';;') ]
                        if len(dd)>i:
                            toadd = dd[i]
                    data = data + (toadd, )
            data = data + (mine['Radioactivity'], mine['Comments'])
            if len(data)!=63:
                print(data)
                tkinter.messagebox.showerror("ERROR!", "Key %s parsed in %d values instead of 64!" % (mine['UID'], len(data)))
                conn.close()
            mindata.append(data)
        c.executemany("INSERT INTO MINERALS (MINID, NAME, LOCALITY, LOCID_MNDAT, SIZE, WEIGHT, ACQUISITION, COLLECTION, VALUE, S1_SPECIES, S1_CLASS, S1_CHEMF, S1_COLOR, S1_FLSW, S1_FLMW, S1_FLLW, S1_FL405, S1_PHSW, S1_PHMW, S1_PHLW, S1_PH405, S1_TENEBR, S2_SPECIES, S2_CLASS, S2_CHEMF, S2_COLOR, S2_FLSW, S2_FLMW, S2_FLLW, S2_FL405, S2_PHSW, S2_PHMW, S2_PHLW, S2_PH405, S2_TENEBR, S3_SPECIES, S3_CLASS, S3_CHEMF, S3_COLOR, S3_FLSW, S3_FLMW, S3_FLLW, S3_FL405, S3_PHSW, S3_PHMW, S3_PHLW, S3_PH405, S3_TENEBR, S4_SPECIES, S4_CLASS, S4_CHEMF, S4_COLOR, S4_FLSW, S4_FLMW, S4_FLLW, S4_FL405, S4_PHSW, S4_PHMW, S4_PHLW, S4_PH405, S4_TENEBR, RADIOACT, COMMENTS) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);", mindata)
        conn.commit()
        conn.close()



def main_gui():
    root = tkinter.Tk()
    MineralApp(root)
    root.mainloop()

if __name__ == "__main__":
    main_gui()


