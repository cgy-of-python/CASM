import idlelib.colorizer as idc
import idlelib.percolator as idp
import os
import sys
import threading
import time
import tkinter as T_tk
import tkinter.filedialog as tf
import tkinter.messagebox as tm
import tkinter.scrolledtext as ts
import tkinter.font as font
import tkinter.ttk
import re
import ttkbootstrap as tk

# version
version = '0.8.7'
running = 0


class SettingFont:
    def __init__(self):
        self.__name = r"tools\cgy-tool-stg.def"
        try:
            with open(self.__name, encoding='utf-8') as a:
                self.font = a.readline().replace('\n', '')
                self.size = int(a.readline().replace('\n', ''))
                if self.size > 250 or self.size < 5:
                    raise ValueError
                self.bold = a.readline().replace('\n', '')
                if self.bold == '':
                    raise ValueError

        except (ValueError, OSError):
            self.font = 'Consolas'
            self.size = 20
            self.bold = 'normal'
            try:
                with open(self.__name, 'w', encoding='utf-8') as a:
                    a.write(f'{self.font}\n{int(self.size)}\n{self.bold}')
            except OSError:
                def a(path):
                    b = os.path.basename(path)
                    c = len(list(b))
                    return path[:-1 * c]

                os.mkdir(f'{a(__file__)}tools\\')
                with open(self.__name, 'w', encoding='utf-8') as a:
                    a.write(f'{self.font}\n{int(self.size)}\n{self.bold}')

    def sf_return(self):
        return self.font, self.size, self.bold

    def shange(self, _font, size, bold):
        with open(self.__name, 'w', encoding='utf-8') as a:
            a.write(f'{_font}\n{size}\n{bold}')
            self.font = _font
            self.size = size
            self.bold = bold

    def check(self):
        try:
            with open(self.__name, 'r', encoding='utf-8') as a:
                _a = a.readline().replace('\n', '')
                if self.font == _a:
                    b = int(a.readline().replace('\n', ''))
                    if self.size == b:
                        if b > 250 or b < 5:
                            raise ValueError
                        c = a.readline().replace('\n', '')
                        if self.bold == c:
                            if c == '':
                                raise ValueError
                            return False
                        else:
                            self.bold = c
                            return True
                    else:
                        self.size = b
                        return True
                else:
                    self.font = _a
                    return True
        except (ValueError, Exception):
            self.font = 'Consolas'
            self.size = 20
            self.bold = 'normal'
            try:
                with open(self.__name, 'w', encoding='utf-8') as a:
                    a.write(f'{self.font}\n{int(self.size)}\n{self.bold}')
            except OSError:
                def a(path):
                    _b = os.path.basename(path)
                    _c = len(list(_b))
                    return path[:-1 * _c]

                os.mkdir(f'{a(__file__)}tools\\')
                with open(self.__name, 'w', encoding='utf-8') as a:
                    a.write(f'{self.font}\n{int(self.size)}\n{self.bold}')
            return True


class SettingColor:
    def __init__(self):
        pass


class GUI:
    def __init__(self):
        # vars
        self.a = None
        self.b = None
        self.c = None
        self.d = None
        self.p = None
        self.i = 0
        # wgts

        self.root = T_tk.Tk()
        self.root.title('CASM 文本编辑器')
        # Settings about fonts
        self.fs = SettingFont()
        self.font, self.size, self.bold = self.fs.sf_return()
        # Settings about colors
        self.cs = SettingColor()
        self.bg = "#ffffff"
        self.normal = '#000000'
        self.numbers = "#FFA500"
        self.comment = "#C0C0C0"
        self.keywords = "#00BFFF"
        self.builtin = "#7B68EE"
        self.string = "#FFA500"
        self.definition = "#228B22"
        self.symbol = "#FF0000"
        self.classmode = "#DA70D6"
        self.line = "#717171"
        # self.root.geometry('1500x900')
        # self.root.minsize(1500, 0)
        self.sb = tk.Scrollbar(self.root, orient=tk.HORIZONTAL)
        self.sb.pack(side=tk.BOTTOM, fill=tk.X)
        self.sb2 = tk.Scrollbar(self.root)
        self.sb2.pack(side=tk.RIGHT, fill=tk.Y)
        self.fall = tk.IntVar(self.root)
        self.text = tk.Text(self.root, undo=True, xscrollcommand=self.sb.set, yscrollcommand=self.sb2.set, wrap='none')
        self.text.config(font=(self.font, self.size, self.bold), background=self.bg, foreground=self.normal)
        self.numl = tk.Canvas(self.root, width=60, bg='white', highlightthickness=0)
        self.numl.pack(side=tk.LEFT, fill=tk.BOTH)
        self.text.pack(side=tk.LEFT, fill=tk.BOTH, expand=tk.YES)
        self.colorful = idc.color_config(self.text)
        self.sb.config(command=self.text.xview)
        self.menu = tk.Menu(self.root)
        self.popup = tk.Menu(self.root, tearoff=0)
        self.filemenu = tk.Menu(self.menu, tearoff=False)
        self.editmenu = tk.Menu(self.menu, tearoff=False)
        self.elsemenu = tk.Menu(self.menu, tearoff=False, selectcolor='green')
        self.saved = False
        self.running = False
        self.myrun = True
        self.installing = False
        self.name = ''
        self.lineall = 0
        self.old = 0

    def loadfile(self, file):
        file = '\n'.join((item.decode('gbk') for item in file)).split('\n')[0]
        self.open(name=file)

    def edit(self, text):
        self.text.delete(1.0, tk.END)
        self.text.insert(tk.END, text)

    def get(self):
        return self.text.get(1.0, tk.END).rstrip()

    def adds(self):
        self.fall.set(1)  # 0开1关,默认关
        idc.color_config(self.text)
        self.p = idp.Percolator(self.text)
        self.text.focus_set()
        # windnd.hook_dropfiles(self.root, func=self.loadfile)
        self.check()

        def the_function_that_which_is_running_at_tkinter_key_press(event):
            self.redraw()
            self.see()
            self.redraw()
            if self.saved:
                self.save()
            self.character_completion(event)

        def the_function_that_which_is_running_at_tkinter_ctrl_v_press(*args):
            self.text.event_generate("<<Paste>>")
            self.see()

        self.root.bind("<Key>", lambda event: the_function_that_which_is_running_at_tkinter_key_press(event))
        self.text.bind('<Tab>', self.tab)
        self.text.bind('<Control-Tab>', self.untab)
        self.text.bind("<Return>", self.enter)
        self.text.bind("<BackSpace>", self.backspace)
        self.text.bind("<Button-1>", self.redraw)
        self.text.bind("<B2-Motion>", self.redraw)
        self.text.bind("<MouseWheel>", self.redraw)
        self.root.bind("<Configure>", self.redraw)
        self.root.bind("<B1-Motion>", self.redraw)
        try:
            self.root.iconbitmap(r'images\lego.ico')
        except tkinter.TclError:
            pass
        font_ = font.Font(root=self.root, font=self.text['font'])
        tab_width = font_.measure(' ' * 4)
        self.text.config(tabs=(tab_width,))
        self.root.protocol("WM_DELETE_WINDOW", self.when_exit_do)
        self.text.tag_configure('Found', background='black', foreground='white', underline=True)
        self.menu.add_cascade(label='文件', menu=self.filemenu)
        self.menu.add_cascade(label='编辑', menu=self.editmenu)
        self.menu.add_cascade(label='其他', menu=self.elsemenu)
        self.filemenu.add_command(label='新建     Ctrl+n', command=self.mynew)
        self.filemenu.add_command(label='保存     Ctrl+s', command=self.save)
        self.filemenu.add_command(label='另存为     Ctrl+e', command=self.saveas)
        self.filemenu.add_command(label='打开文件     Ctrl+f', command=self.open)
        self.filemenu.add_command(label='运行     Ctrl+r', command=self.runtext)
        self.filemenu.add_command(label='调试     F5', command=self.debugtext)
        self.filemenu.add_command(label='退出     Ctrl+q', command=self.when_exit_do)
        self.editmenu.add_command(label='替换     Ctrl+Alt+r', command=self.replace)
        self.editmenu.add_command(label='寻找     Ctrl+Alt+f', command=self.find)
        self.editmenu.add_command(label='字数统计     Ctrl+Alt+t', command=self.count)
        self.editmenu.add_command(label='设置     Ctrl+Alt+s', command=self.changestg)
        self.elsemenu.add_command(label='快捷键&帮助     Ctrl+Alt+e', command=self.elsekey)
        self.elsemenu.add_command(label='关于     Ctrl+Alt+a',
                                  command=lambda: tm.showinfo('关于', '本编辑器由保定曹高远制作。原创。'))
        self.elsemenu.add_command(label='关于文件     Ctrl+Alt+f', command=self.about_file)
        self.elsemenu.add_checkbutton(label="全屏     F11", variable=self.fall, offvalue=0, onvalue=1,
                                      command=lambda: self._falls())
        self.root.bind('<Control-n>', lambda event: self.mynew())
        self.root.bind('<Control-s>', lambda event: self.save())
        self.root.bind('<Control-e>', lambda event: self.saveas())
        self.root.bind('<Control-r>', lambda event: self.runtext())
        self.root.bind('<Control-f>', lambda event: self.open())
        self.root.bind('<F5>', lambda event: self.debugtext())
        self.root.bind('<Control-q>', lambda event: self.when_exit_do())
        self.root.bind('<Control-Alt-r>', lambda event: self.replace())
        self.root.bind('<Control-Alt-f>', lambda event: self.find())
        self.root.bind('<Control-Alt-t>', lambda event: self.count())
        self.root.bind('<Control-Alt-s>', lambda event: self.changestg())
        self.root.bind('<Control-Alt-e>', lambda event: self.elsekey())
        self.root.bind('<Control-Alt-f>', lambda event: self.about_file())
        self.root.bind('<Control-Alt-a>', lambda event: tm.showinfo('关于', '本编辑器由伟大的天才曹高远制作。'))
        self.root.bind('<Control-MouseWheel>', lambda event: self.bigorsmall_font(event))
        self.root.bind('<F11>', lambda event: self.falls())
        self.popup.add_command(label='复制', command=lambda: self.text.event_generate("<<Copy>>"))
        self.popup.add_command(label='剪切', command=lambda: self.text.event_generate("<<Cut>>"))
        self.popup.add_command(label='粘贴', command=the_function_that_which_is_running_at_tkinter_ctrl_v_press)
        self.root.bind("<Button-3>", self.popup_menu)
        self.root.config(menu=self.menu)

    def falls(self):
        a = self.fall.get()
        if a == 1:
            self.root.attributes("-fullscreen", False)
            self.fall.set(0)  # 改变状态
        else:
            self.root.attributes("-fullscreen", True)
            self.fall.set(1)

    def _falls(self):
        a = self.fall.get()
        if a == 0:
            self.root.attributes("-fullscreen", False)  # 不改变状态
        else:
            self.root.attributes("-fullscreen", True)

    def see(self):
        self.text.see(str(float(int(float(self.text.index('insert'))) + 2)))  # 一直显示光标所在行

    def __update__(self, *args):
        while True:
            self.root.update()
            new = str(int(float(self.text.index('insert'))))
            if new != self.old:
                self.old = new
                self.redraw()
            try:
                if self.fs.check():
                    self.font, self.size, self.bold = self.fs.sf_return()
                    self.update()
            except RuntimeError:
                break

    def insert_get(self):
        start = tk.SEL_FIRST
        end = tk.SEL_LAST
        is_insert = True if self.text.get(start, end) != '' else False
        return is_insert, self.text.index(start), self.text.index(end)

    def update(self):
        self.text.config(font=(self.font, self.size, self.bold))
        self.redraw()

    def tab(self, *args):
        is_insert, start, end = self.insert_get()
        if not is_insert:
            self.text.insert('insert', '    ')
        else:
            start = int(float(start))
            end = int(float(end))
            for i in range(start, end + 1):
                self.text.insert(str(float(i)), '    ')
        return 'break'

    def untab(self, *args):
        is_insert, start, end = self.insert_get()
        if not is_insert:
            last = str(int(''.join(list(self.text.index('insert'))[
                                   (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 4)
            all_insert = ''.join(list(self.text.index('insert'))[
                                 :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last
            if self.text.get(all_insert, 'insert') == '    ':
                self.text.delete(all_insert, 'insert')
        else:
            start = self.text.index(start)
            end = self.text.index(end)
            for i in range(int(float(start)), int(float(end)) + 1):
                last = str(int(''.join(list(self.text.index(str(float(i))))[
                                       (-1 * (len(os.path.splitext(self.text.index(str(float(i))))[-1]) - 1)):])) + 4)
                all_insert = ''.join(list(self.text.index(str(float(i))))[
                                     :(-1 * (len(os.path.splitext(self.text.index(str(float(i))))[-1]) - 1))]) + last
                if self.text.get(str(float(i)), all_insert) == '    ':
                    self.text.delete(str(float(i)), all_insert)
        return 'break'

    def popup_menu(self, event):
        self.popup.post(event.x_root, event.y_root)  # 弹出右键菜单

    def mynew(self):
        GUI().run()

    def redraw(self, *args):
        try:
            global linenum
            self.numl.delete("all")
            i = self.text.index("@0,0")
            while True:
                dline = self.text.dlineinfo(i)
                if dline is None:
                    break
                y = dline[1]
                linenum = str(i).split(".")[0]
                self.numl.create_text(2, y, anchor="nw", text=linenum, fill=self.line,
                                      font=(self.font, self.size, self.bold))
                i = self.text.index("%s+1line" % i)

            _font = font.Font(root=self.root, family=self.font, size=self.size)
            w = _font.measure('0')
            self.numl.config(width=len(linenum) * w + 5)
            self.lineall = linenum
        except RuntimeError:
            pass

    def save(self):
        if self.name == '':
            self.name = tf.asksaveasfilename(initialfile='main.casm', defaultextension='.casm',
                                             filetypes=(('CASM文件', ('*.casm', '*.txt')), ('所有文件', '*.*')))
            if self.name == '':
                return
            try:
                with open(self.name, 'w', encoding='utf-8') as casm:
                    casm.write(self.text.get(1.0, tk.END)[:-1])
            except:
                self.name = ''
            else:
                self.saved = True
                self.root.title(self.name)
        else:
            try:
                with open(self.name, 'w', encoding='utf-8') as casm:
                    casm.write(self.text.get(1.0, tk.END)[:-1])
            except OSError:
                pass

    def saveas(self):
        if self.saved:
            if self.running:
                tm.showerror('文本编辑器', '文件正在运行，请先关闭。')
                return
        self.name = tf.asksaveasfilename(initialfile='main.casm', defaultextension='.casm',
                                         filetypes=(('CASM文件', ('*.casm', '*.txt')), ('所有文件', '*.*')))
        if self.name == '':
            return
        try:
            with open(self.name, 'w', encoding='utf-8') as casm:
                casm.write(self.text.get(1.0, tk.END)[:-1])
        except:
            self.name = ''
        else:
            self.saved = True
            self.root.title(self.name)

    def highins(self, text: str):
        self.text.insert(tk.END, text)

    def open(self, name=''):
        if self.running:
            tm.showerror('文本编辑器', '文件正在运行，请先关闭。')
            return
        if name == '':
            if self.name == '':
                self.name = tf.askopenfilename(
                    filetypes=(("CASM文件", ("*.casm", '*.txt')), ("所有文件", "*.*")))
                if self.name == '':
                    return
                try:
                    with open(self.name, encoding='utf-8') as casm:
                        self.text.delete(1.0, tk.END)
                        self.highins(casm.read())
                except UnicodeError:
                    self.saved = False
                    self.redraw()
                    self.name = ''
                    self.root.title('CASM 文本编辑器')
                    tm.showerror('文本编辑器', '我们只支持编码为utf-8文件。')
                except PermissionError:
                    self.saved = False
                    self.redraw()
                    self.name = ''
                    self.root.title('CASM 文本编辑器')
                    tm.showerror('Error', '权限不足，访问被拒绝')
                else:

                    self.check()
                    self.redraw()
                    self.saved = True
                    self.root.title(self.name)
                    self.text.edit_reset()  # 重置文本框的撤销功能
            else:
                a = tm.askokcancel(self.name, '你已打开了一个文件,要把它关闭并保存在打开新文件吗？')
                if a:
                    self.name = tf.askopenfilename(
                        filetypes=(("CASM文件", ("*.casm", '*.txt')), ("所有文件", "*.*")))
                    if self.name == '':
                        return
                    try:
                        with open(self.name, encoding='utf-8') as casm:
                            self.text.delete(1.0, tk.END)
                            self.highins(casm.read())
                    except UnicodeError:
                        self.saved = False
                        self.redraw()
                        self.name = ''
                        self.root.title('CASM 文本编辑器')
                        tm.showerror('文本编辑器', '我们只支持编码为utf-8文件。')
                    except PermissionError:
                        self.saved = False
                        self.redraw()
                        self.name = ''
                        self.root.title('CASM 文本编辑器')
                        tm.showerror('Error', '权限不足，访问被拒绝')
                    else:
                        self.check()
                        self.redraw()
                        self.saved = True
                        self.root.title(self.name)
                        self.text.edit_reset()
        else:
            if self.name == '':
                try:
                    with open(name, encoding='utf-8') as casm:
                        self.text.delete(1.0, tk.END)
                        self.highins(casm.read())
                except UnicodeError:
                    self.saved = False
                    self.redraw()
                    self.root.title('CASM 文本编辑器')
                    tm.showerror('文本编辑器', '我们只支持编码为utf-8文件。')
                except PermissionError:
                    self.saved = False
                    self.redraw()
                    self.root.title('CASM 文本编辑器')
                    tm.showerror('Error', '权限不足，访问被拒绝')
                except OSError:
                    self.saved = False
                    self.redraw()
                    self.text.delete(1.0, tk.END)
                    self.text.insert(tk.END, name)
                else:
                    self.name = name
                    self.check()
                    self.redraw()
                    self.saved = True
                    self.root.title(self.name)
                    self.text.edit_reset()
            else:
                a = tm.askokcancel(self.name, '你已打开了一个文件,要把它关闭并保存在打开新文件吗？')
                if a:
                    try:
                        with open(name, encoding='utf-8') as casm:
                            self.text.delete(1.0, tk.END)
                            self.highins(casm.read())
                    except UnicodeError:
                        self.saved = False
                        self.redraw()
                        self.root.title('CASM 文本编辑器')
                        tm.showerror('文本编辑器', '我们只支持编码为utf-8文件。')
                    except PermissionError:
                        self.saved = False
                        self.redraw()
                        self.root.title('CASM 文本编辑器')
                        tm.showerror('Error', '权限不足，访问被拒绝')
                    except OSError:
                        self.saved = False
                        self.redraw()
                        self.text.delete(1.0, tk.END)
                        self.text.insert(tk.END, name)
                    else:
                        self.name = name
                        self.check()
                        self.redraw()
                        self.saved = True
                        self.root.title(self.name)
                        self.text.edit_reset()

    def make_pat(self):
        def _any(name, alternates):
            return "(?P<%s>" % name + "|".join(alternates) + ")"
        kwlist = ["FUNC", "VAR", "IFE", "IFB", "IFS", "GOTO", "IF", "RET"]
        kw = r"\b" + _any("KEYWORD", kwlist) + r"\b"
        builtinlist = ["TYPE", "PUSH", "OUT", "ENDL", "PUSHCOMMENT", "POP", "ADD", "SUB", "MUL", "DIV", "MOD", "IN",
                       "GETVAR", "PUSHVAR", "EXIT", "RAND", "NOT", "CLEAR", "STR", "INT", "BOOL", "DOUBLE", "FLOAT",
                       "CHAR", "GTVAR", "NEWVAR", "FOR"]
        # builtin = r"([^.'\"\\#]\b|^)" + _any("BUILTIN", builtinlist) + r"\b"
        builtin = r"\b" + _any("BUILTIN", builtinlist) + r"\b"
        comment = _any("COMMENT", [r";[^\n]*"])
        number = _any("NUM", [r"\b[0-9]+(?![0-9]+)\b"])
        prog = re.compile("|".join([
            builtin, comment, kw, number,
            _any("SYNC", [r"\n"]),
        ]),
            re.DOTALL | re.MULTILINE)
        return prog

    def check(self):
        if (os.path.splitext(self.name)[-1] in ['.casm', '.txt']) or self.name == '':
            try:
                self.p.removefilter(self.d)
            except AssertionError:
                pass
            self.d = idc.ColorDelegator()
            self.d.prog = self.make_pat()
            self.d.tagdefs['COMMENT'] = {'foreground': self.comment}
            self.d.tagdefs['KEYWORD'] = {'foreground': self.keywords}
            self.d.tagdefs['BUILTIN'] = {'foreground': self.builtin}
            self.d.tagdefs['NUM'] = {'foreground': self.numbers}
            try:
                self.p.insertfilter(self.d)
            except AssertionError:
                pass
        else:
            try:
                self.p.removefilter(self.d)
            except AssertionError:
                pass
            self.d = idc.ColorDelegator()
            self.d.prog = self.make_pat()
            self.d.tagdefs['COMMENT'] = {'foreground': self.normal}
            self.d.tagdefs['KEYWORD'] = {'foreground': self.normal}
            self.d.tagdefs['BUILTIN'] = {'foreground': self.normal}
            self.d.tagdefs['NUM'] = {'foreground': self.normal}
            try:
                self.p.insertfilter(self.d)
            except AssertionError:
                pass

    def runtext(self):
        if self.saved:
            if not self.running:
                self.save()
                if self.name[-5:] == '.casm':
                    path = self.name.split('/')
                    path = '\\'.join(path[:-1])
                    cmd = f'casm.exe \"{self.name}\" & pause'
                elif self.name[-4:] == '.txt':
                    path = self.name.split('/')
                    path = '\\'.join(path[:-1])
                    cmd = f'casm.exe \"{self.name}\"'
                else:
                    cmd = ''

                def aa():
                    self.running = True
                    print("cmd:", cmd)
                    os.system(cmd)
                    self.running = False

                self.a = threading.Thread(target=aa)
                self.a.start()
            else:
                tm.showerror(self.name, '文件已运行')
        else:
            self.mycallsave()

    def debugtext(self):
        if self.saved:
            if not self.running:
                self.save()
                print(self.name[-5:])
                if self.name[-5:] == '.casm':
                    path = self.name.split('/')
                    path = '\\'.join(path[:-1])
                    cmd = f'casm.exe \"{self.name}\" --debug & pause'  # 加‘&’连续执行cmd命令
                elif self.name[-4:] == '.txt':
                    path = self.name.split('/')
                    path = '\\'.join(path[:-1])
                    cmd = f'casm.exe \"{self.name}\" --debug'
                else:
                    cmd = ''

                def aa():
                    self.running = True
                    print("cmd:", cmd)
                    os.system(cmd)
                    self.running = False

                self.a = threading.Thread(target=aa)
                self.a.start()
            else:
                tm.showerror(self.name, '文件已运行')
        else:
            self.mycallsave()

    def mycallsave(self):
        tm.showinfo('文本编辑器', '你没有保存这个文件，请先保存。')
        self.save()

    def run(self):
        global running
        running += 1
        self.adds()
        self.redraw()
        self.falls()
        while True:
            self.__update__()

    def enter(self, *args):
        self.i = 0
        a = self.text.index('insert')
        aa = int(float(a))
        b = self.text.get(float(aa), a).replace('\n', '')
        c = b
        if b[0:5] == 'FUNC ':
            i = 0
            while True:
                if b[:4] == '    ':
                    b = b[4:]
                    i += 1
                else:
                    break
            self.i = i + 1
        else:
            i = 0
            while True:
                if b[:4] == '    ':
                    b = b[4:]
                    i += 1
                else:
                    break
            self.i = i
            try:
                a = c.strip().split()[0]
            except IndexError:
                a = ''
        self.text.insert('insert', '\n')
        for j in range(self.i):
            self.text.insert('insert', '    ')
        self.redraw()
        self.see()
        return 'break'

    def backspace(self, *args):
        _dict = {'(': ')', '{': '}', '[': ']', '"': '"', "'": "'"}
        last = str(int(''.join(list(self.text.index('insert'))[
                               (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 1)
        all_insert = ''.join(list(self.text.index('insert'))[
                             :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last
        a = self.text.get(all_insert, 'insert')
        if a in ['(', '{', '[', '"', "'"]:
            last = str(int(''.join(list(self.text.index('insert'))[
                                   (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) + 1)
            all_insert = ''.join(list(self.text.index('insert'))[
                                 :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last
            b = self.text.get('insert', all_insert)
            if b == _dict[a]:  # 符合条件，删除2个
                last1 = str(int(''.join(list(self.text.index('insert'))[
                                        (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 1)
                all_insert1 = ''.join(list(self.text.index('insert'))[
                                      :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last1
                last2 = str(int(''.join(list(self.text.index('insert'))[
                                        (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) + 1)
                all_insert2 = ''.join(list(self.text.index('insert'))[
                                      :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last2
                self.text.delete(all_insert1, all_insert2)
                return 'break'
        else:
            last1 = str(int(''.join(list(self.text.index('insert'))[
                                    (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 4)
            all_insert1 = ''.join(list(self.text.index('insert'))[
                                  :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last1
            if self.text.get(all_insert1, 'insert') == '    ':
                self.text.delete(all_insert1, 'insert')
                return 'break'

    def character_completion(self, event: T_tk.Event):  # 字符补全（对【(】、【[】、【{】等进行自动补全）
        a = event.keysym
        if a == 'parenleft':  # (
            self.text.insert("insert", ')')
            last = str(int(''.join(list(self.text.index('insert'))[
                                   (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 1)
            all_insert = ''.join(list(self.text.index('insert'))[
                                 :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last
            self.text.mark_set('insert', all_insert)
        elif a == 'braceleft':  # {
            self.text.insert("insert", '}')
            last = str(int(''.join(list(self.text.index('insert'))[
                                   (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 1)
            all_insert = ''.join(list(self.text.index('insert'))[
                                 :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last
            self.text.mark_set('insert', all_insert)
        elif a == 'bracketleft':  # [
            self.text.insert("insert", ']')
            last = str(int(''.join(list(self.text.index('insert'))[
                                   (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 1)
            all_insert = ''.join(list(self.text.index('insert'))[
                                 :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last
            self.text.mark_set('insert', all_insert)
        elif a == 'quotedbl':
            self.text.insert("insert", '"')
            last = str(int(''.join(list(self.text.index('insert'))[
                                   (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 1)
            all_insert = ''.join(list(self.text.index('insert'))[
                                 :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last
            self.text.mark_set('insert', all_insert)
        elif a == "apostrophe":
            self.text.insert("insert", "'")
            last = str(int(''.join(list(self.text.index('insert'))[
                                   (-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1)):])) - 1)
            all_insert = ''.join(list(self.text.index('insert'))[
                                 :(-1 * (len(os.path.splitext(self.text.index('insert'))[-1]) - 1))]) + last
            self.text.mark_set('insert', all_insert)

    '''EDIT TOOLS'''

    def __replace(self, ta, tb, num):
        if ta and tb and num:
            num = int(num)
            if num == -1:
                a = self.get()
                a = a.replace(ta, tb)
                self.edit(a)
            elif num >= 0:
                pass
            else:
                a = self.get()
                a = a.replace(ta, tb, num)
                self.edit(a)
        else:
            tm.showerror('文本编辑器', '文本框不可留空！')

    def replace(self):
        if self.name != '':
            self.save()
            root = tk.Toplevel()
            root.title('替换')
            root.resizable(False, False)
            try:
                root.iconbitmap(r'images\lego.ico')
            except tkinter.TclError:
                pass
            label = tk.Label(root, text=u'原始内容:')
            label.grid(padx=10, pady=10, row=0, column=0)
            entry = tk.Entry(root)
            entry.grid(padx=10, pady=10, row=0, column=1)
            label2 = tk.Label(root, text=u'替换为:')
            label2.grid(padx=10, pady=10, row=1, column=0)
            entry2 = tk.Entry(root)
            entry2.grid(padx=10, pady=10, row=1, column=1)
            label3 = tk.Label(root, text='次数（-1为全部替换）')
            label3.grid(padx=10, pady=10, row=2, column=0)
            entry3 = tk.Entry(root)
            entry3.grid(padx=10, pady=10, row=2, column=1)
            btn = tk.Button(root, text='替换！',
                            command=lambda: self.__replace(entry.get(), entry2.get(), entry3.get()))
            btn.grid(padx=10, pady=10, row=3, column=0)
            root.mainloop()
        else:
            self.mycallsave()

    def __find(self, msg: str):
        if self.name:
            if msg:
                i = 0
                self.text.tag_remove('Found', '1.0', tk.END)
                start = '1.0'
                if len(msg.rstrip()) == 0:
                    return 0
                while True:
                    pos = self.text.search(msg, start, tk.END)
                    if pos == '':
                        break
                    i += 1
                    self.text.tag_add('Found', pos, '%s+%dc' % (pos, len(msg)))
                    start = '%s+%dc' % (pos, len(msg))
                return i
            else:
                tm.showerror('文本编辑器', '文本框不可留空！')
        else:
            self.mycallsave()

    def delatefind(self):
        self.text.tag_remove('Found', '1.0', tk.END)

    def runfind(self, msg):
        a = self.__find(msg)
        tm.showinfo(self.name, f'共找到"{msg}"{a}个。')

    def find(self):
        if self.name != '':
            self.save()
            root = tk.Toplevel()
            root.title('查找')
            try:
                root.iconbitmap(r'images\lego.ico')
            except tkinter.TclError:
                pass
            root.resizable(False, False)
            entry = tk.Entry(root)
            entry.grid(row=1, column=0, pady=10, padx=10)
            btn1 = tk.Button(root, text='寻找', command=lambda: self.runfind(entry.get().rstrip()))
            btn1.grid(row=1, column=1, padx=10, pady=10)
            btn2 = tk.Button(root, text='清空', command=self.delatefind)
            btn2.grid(row=1, column=2, padx=10, pady=10)
            root.mainloop()

    def count(self):
        '''字数统计'''
        if self.name != '':
            self.save()
            a = self.get()
            a = len(list(a))
            tm.showinfo('字数统计', f'文件{self.name}有字符{a}个')
        else:
            self.mycallsave()

    def bigorsmall_font(self, event):
        if event.delta > 0:
            if self.size < 250:
                self.size += 1
                self.text.config(font=(self.font, self.size, self.bold))
                self.redraw()
                font_ = font.Font(root=self.root, font=self.text['font'])
                tab_width = font_.measure(' ' * 4)
                self.text.config(tabs=tab_width)
            else:
                tm.showerror('CASM编辑器', '字体过大')
        else:
            if self.size > 5:
                self.size -= 1
                self.text.config(font=(self.font, self.size, self.bold))
                self.redraw()
                font_ = font.Font(root=self.root, font=self.text['font'])
                tab_width = font_.measure(' ' * 4)
                self.text.config(tabs=tab_width)
            else:
                tm.showerror('CASM编辑器', '字体过小')
        self.fs.shange(self.font, int(self.size), self.bold)

    def changestg(self):
        root = tk.Toplevel()
        root.title('设置')
        root.geometry('300x250')
        try:
            root.iconbitmap(r'images\lego.ico')
        except tkinter.TclError:
            pass
        root.resizable(False, False)
        notepad = tkinter.ttk.Notebook(root)
        fr1 = tk.Frame(root)
        notepad.add(fr1, text='设置')
        var = tk.StringVar(root)
        var3 = tk.BooleanVar(root)
        var3.set(True if self.bold == "bold" else False)
        fontsfam = sorted([i for i in font.families(root) if not i.startswith("@")], reverse=True)
        fontsfam = tuple(fontsfam)

        var.set(self.font)
        var2 = tk.IntVar(root)
        var2.set(self.size)
        label = tk.Label(fr1, text=u'字体：')
        label.grid(row=0, column=0, padx=10, pady=10)
        cb = tkinter.ttk.Combobox(fr1, textvariable=var, values=fontsfam, width=30,
                                  state='readonly')  # state='readonly':将下拉列表设置成为只读模式
        cb.grid(row=0, column=1, pady=10, padx=10)
        label2 = tk.Label(fr1, text='大小')
        label2.grid(row=1, column=0, pady=10, padx=10)
        sl = tk.Scale(fr1, from_=1, to=100, orient=tk.HORIZONTAL, length=250, variable=var2)
        sl.grid(row=1, column=1, padx=10, pady=10)
        label3 = tk.Label(fr1, text='是否粗体')
        label3.grid(row=2, column=0, pady=10, padx=10)
        ck = tk.Checkbutton(fr1, text='使用粗体', variable=var3)
        ck.grid(row=2, column=1, padx=10, pady=10)

        def a():
            aa = var.get().rstrip()
            bb = var2.get()
            cc = var3.get()
            self.fs.shange(aa, bb, "bold" if cc else "normal")
            self.font = aa
            self.size = bb
            self.bold = "bold" if cc else "normal"
            self.text.config(font=(self.font, self.size, self.bold))
            font_ = font.Font(root=self.root, font=self.text['font'])
            tab_width = font_.measure(' ' * 4)
            self.text.config(tabs=(tab_width,))
            root.destroy()

        btn = tk.Button(fr1, text='确认', command=a)
        btn.grid(row=3, column=0, padx=10, pady=10)
        fr2 = tk.Frame(root)
        notepad.add(fr2, text='预览')
        sb = tk.Scrollbar(fr2, orient=tk.HORIZONTAL)
        sb.pack(side=tk.BOTTOM, fill=tk.X)
        text = ts.ScrolledText(fr2, width=300, height=200, wrap='none', xscrollcommand=sb.set)

        def textv(*args):
            text.config(font=(var.get().rstrip(), var2.get(), "bold" if var3.get() else "normal"))
            root.geometry('%dx%d' % (600, 550))

        def labelv(*args):
            root.geometry('%dx%d' % (350, 220))

        text.bind('<Visibility>', textv)
        label.bind('<Visibility>', labelv)
        text.insert(tk.END, '''
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
CASM editor is a small and lightweight editor.
CASM editor是一个小巧轻量的编辑器。
CASM Editor - это небольшой и легкий редактор.
CASM editorは小型軽量のエディタです。
CASM editor는 작고 가벼운 편집기입니다.
CASM တည်းဖြတ်ချက်ဟာ ချိတ်ဆက်နဲ့ လျှော့လင့်တဲ့ တည်းဖြတ်ချက်ပါ။
Casm Editor est un éditeur petit et léger.
Editor CASM editor compact and lightweight editor est.
El editor casm es un editor pequeño y ligero.
CASM editor é um editor compacto e leve.
 المستخلصات المحرر هو صغير وخفيف الوزن المحرر . 
CASM सम्पादकम्य सङ्केतं चित्रकं सम्पादकम्
ເຄື່ອງມືແກ້ໄຂCAS ເປັນຜູ້ກ່ຽວກັບຄຳແກ້ກແລະຄຳແກ້ໄຫລາຍດິນ
        ''')
        text.config(font=(var.get().rstrip(), var2.get(), "bold" if var3.get() else "normal"))
        sb.config(command=text.xview)
        text.pack()
        notepad.pack()
        root.mainloop()

    def elsekey(self):
        root = tk.Toplevel()
        root.title('其他')
        text = ts.ScrolledText(root)
        try:
            root.iconbitmap(r'images\lego.ico')
        except tkinter.TclError:
            pass
        text.config(font=(self.font, self.size, self.bold))
        text.insert(1.0,"""0.注释:用于在代码中添加注释。注释不会被执行，只是对代码的解释说明。
    ;注释内容
1.FUNC:通过这个指令，你可以定义一个可以被跳转的代码区块。
    FUNC [NAME]
    *结束标记：另一个函数或文件结束
2.TYPE:这个指令用于标明要保存的数据的类型。
    TYPE [INT（高精整数） | STR（字符串） | BOOL（布尔值） | DOUBLE（双精度浮点数） | FLOAT（浮点数） | CHAR（字符）]
3.PUSH:这个指令用于将数据推入栈顶。
    PUSH [DATA]
4.OUT:用于将栈顶的数据输出。
    OUT
5.ENDL:用于输出一个换行符。
    ENDL
6.PUSHCOMMENT:将一个分号字符";"推入字符栈的栈顶。
    PUSHCOMMENT
7.POP:从栈中移除栈顶元素。
    POP
8.ADD:取出栈顶的元素，与下一个栈顶元素相加，并将结果推回栈顶。对于布尔类型，加法表示逻辑“或”操作。
    ADD
9.SUB:取出栈顶的元素，与下一个栈顶元素相减，并将结果推回栈顶。对于布尔类型，减法表示逻辑“异或”操作。对于字符串类型，减法会导致错误。
    SUB
10.MUL:取出栈顶的元素，与下一个栈顶元素相乘，并将结果推回栈顶。对于布尔类型，乘法表示逻辑“与”操作。对于字符串类型，乘法会导致错误。
    MUL
11.DIV:取出栈顶的元素，与下一个栈顶元素相除，并将结果推回栈顶。字符串、布尔类型或字符类型无法进行除法操作。
    DIV
12.MOD: 取出栈顶的元素，与下一个栈顶元素进行取模运算，并将结果推回栈顶。仅适用于整数类型。
    MOD
13.IN:从输入中读取一个变量的值，并将其推入栈顶。
    IN
14.VAR:声明一个变量，如果变量已存在，则不变。
    VAR [NAME]
15.GETVAR:查找名为 [VARNAME] 的变量，然后将其值赋给栈顶元素，并弹栈。
    GETVAR [VARNAME]
16.PUSHVAR:将变量推入栈顶。
    PUSHVAR [VARNAME]
17.IFE:比较栈顶元素与次栈顶元素是否相等，如果相等，将真值推入布尔栈。
    IFE
18.IFB:比较栈顶元素与次栈顶元素，判断是否栈顶元素较大，如果是，将真值推入布尔栈。
    IFB
19.IFS:比较栈顶元素与次栈顶元素，判断是否次栈顶元素较大，如果是，将真值推入布尔栈。
    IFS
20.GOTO:跳转到指定的代码块。
    GOTO [NAME]
21.IF: 如果布尔栈的栈顶为真，跳转到指定标签。如果标签是 "RET"，函数将退出。
    IF [NAME]
22.EXIT:退出整个程序。
    EXIT
23.RAND: 将一个指定范围内的随机整数推入整数栈。
    RAND [NUM1] [NUM2]
    *从 NUM1 开始，到 NUM1+NUM2 结束
24.CLEAR: 清空栈中所有的数据。
    CLEAR
25.NOT:对布尔栈顶进行“非”运算。
    NOT
26.GTVAR:接受一个string形VAR，然后GOTO到该变量的值。
    条件：TYPE STR
    GTVAR [VARNAME]
27.NEWVAR:新建一个VAR，如果VAR已经存在，则覆盖为0。
    NEWVAR [VARNAME]
28.FOR:将int变量[VARNAME]设为[START],再累加到[END],每次累加1，并执行函数[FUNCNAME]。
    PUSH [END]
    PUSH [START]
    FOR [VARNAME] [FUNCNAME]""")
        text.insert(tk.END, '\n*****************快捷键*******************\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '1. 复制   Ctrl+c\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '2. 粘贴   Ctrl+v\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '3. 剪切   Ctrl+x\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '4. 重做   Ctrl+z\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '5. 返回   Ctrl+y\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '6. 全选   Ctrl+a\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '******************作者*******************\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '1. 他叫曹高远，来自河北保定\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '\n')
        text.insert(tk.END, '\n')
        text.configure(state=tk.DISABLED)
        text.pack(side=tk.BOTTOM, fill=tk.BOTH)
        root.mainloop()

    def when_exit_do(self):
        global running
        if not self.running:
            if self.saved:
                self.save()
                if self.saved:
                    self.root.destroy()
                    self.myrun = False
                    if running == 1:
                        sys.exit(0)
                    else:
                        running -= 1

            else:
                a = tm.askyesnocancel('CASM文本编辑器', '你没有保存文件，是否保存后再退出？')
                if a:
                    self.save()
                    if self.saved:
                        self.root.destroy()
                        self.myrun = False
                        if running == 1:
                            sys.exit(0)
                        else:
                            running -= 1
                elif a is None:
                    pass
                else:
                    self.myrun = False
                    self.root.destroy()
                    self.myrun = False
                    if running == 1:
                        sys.exit(0)
                    else:
                        running -= 1
        else:
            tm.showerror('文本编辑器', '文件正在运行，请先关闭。')

    def about_file(self):
        if self.name != '':
            root = tk.Toplevel()
            root.title('关于文件')
            root.geometry('600x500')
            try:
                root.iconbitmap(r'images\lego.ico')
            except tkinter.TclError:
                pass
            label = tk.Label(root)
            string = f"""
    文件名： {os.path.basename(self.name)}
    文件路径： {self.name}
    拓展名： {os.path.splitext(self.name)[-1]}
    大小： {round(os.path.getsize(self.name) / 1024, 2)} kb （{round(os.path.getsize(self.name), 2)} b）

    """
            label.config(text=string)
            label.pack()
            root.mainloop()

        else:
            self.mycallsave()


def check():
    system = os.name
    if system != 'nt':
        tm.showerror('ERROR!', '本程序只支持Microsoft Windows系统。')
        raise SystemExit('SYSTEM ERROR!')
    root = T_tk.Tk()
    root.withdraw()
    if 'casm.exe' in os.listdir(os.path.dirname(os.path.abspath(__file__))):

        tm.showinfo('恭喜！', '一切正常，您可以使用本编辑器！')
    else:
        tm.showerror('错误', '请确保您的电脑上已安装CASM。')
        sys.exit()
    root.destroy()


def hello():
    root = T_tk.Tk()
    root.title('Hello-CASM editor %s' % version)
    root.resizable(False, False)
    root.attributes("-topmost", True)
    try:
        root.iconbitmap(r'images\lego.ico')
    except tkinter.TclError:
        pass
    try:
        img = tk.PhotoImage(file=r'23.4.1/images\lego.png')
        label = tk.Label(root, image=img)
        label.pack()
    except tkinter.TclError:
        pass
    label2 = tk.Label(root, text='正在加载中，请稍后……', font=('宋体', 25))
    label2.pack()
    pr = tkinter.ttk.Progressbar(root, length=700)
    pr['maximum'] = 100
    pr['value'] = 0
    pr.pack()

    def a():
        try:
            for i in range(100):
                pr['value'] += 1
                label2.configure(text=f'正在加载……    (  {i}%  )')
                root.update()
                time.sleep(0.01)
            label2.configure(text=f'正在加载……    (  100%  ),完成！')
            root.update()
            time.sleep(0.001)
        except (tkinter.TclError, RuntimeError, Exception):
            return
        label2.configure(text=f'按窗口内任意位置以继续……')
        root.update()

        def aa(*args, **kwargs):
            root.destroy()

        root.bind('<Button-1>', aa)

    try:
        a()
    except tkinter.TclError:
        return
    root.mainloop()


def new():
    gui = GUI()
    gui.run()


def main():
    try:
        new()
        T_tk.mainloop()
    except (RuntimeError, tkinter.TclError, KeyboardInterrupt):
        sys.exit()


if __name__ == '__main__':
    main()
