from itertools import combinations_with_replacement
from random import shuffle
from os import system

s21_grep = "./s21_grep"
grep = "grep"

s21_res = 's21_grep.res'
original_res = 'grep.res'

regex0 = '[0-9]'
regex1 = 'ok'
regex2 = 'hello'
test_path = 'testing/'
test_file0 = 'test0.txt'
test_file1 = 'test1.txt'
test_file2 = 'test2.txt'
regex_file = 'pattern.txt'
fake_file = 'some.file'

flags = [
    f'-e {regex0} -e {regex1} {test_path + test_file0}',
    f'-ie {regex1} {test_path + test_file0}',
    f'-ve {regex1} {test_path + test_file0}',
    f'-ce {regex1} {test_path + test_file0}',
    f'-le {regex1} {test_path + test_file0} {test_path + test_file1}',
    f'-ne {regex1} {test_path + test_file0}',
    f'-he {regex1} {test_path + test_file0} {test_path + test_file2}',
    f'-se {regex1} {test_path + test_file0} {test_path + fake_file}',
    f'-f {test_path + regex_file} -e {regex2} {test_path + test_file0}',
    f'-oe {regex1} {test_path + test_file0}',
    f'-iv {regex1} {test_path + test_file0}',
    f'-ic {regex1} {test_path + test_file0}',
    f'-il {regex1} {test_path + test_file0} {test_path + test_file1}',
    f'-in {regex1} {test_path + test_file0}',
    f'-ih {regex1} {test_path + test_file0} {test_path + test_file2}',
    f'-is {regex1} {test_path + test_file0} {test_path + fake_file}',
    f'-if {test_path + regex_file} {test_path + test_file0}',
    f'-vc {regex1} {test_path + test_file0}',
    f'-vl {regex1} {test_path + test_file0} {test_path + test_file1}',
    f'-vn {regex1} {test_path + test_file0}',
    f'-vh {regex1} {test_path + test_file0} {test_path + test_file2}',
    f'-vs {regex1} {test_path + test_file0} {test_path + fake_file}',
    f'-vf {test_path + regex_file} {test_path + test_file0}',
    f'-vo {regex1} {test_path + test_file0}',
    f'-cl {regex1} {test_path + test_file0} {test_path + test_file1}',
    f'-cn {regex1} {test_path + test_file0}',
    f'-ch {regex1} {test_path + test_file0} {test_path + test_file2}',
    f'-cs {regex1} {test_path + test_file0} {test_path + fake_file}',
    f'-cf {test_path + regex_file} {test_path + test_file0}',
    f'-co {regex1} {test_path + test_file0}',
    f'-ln {regex1} {test_path + test_file0} {test_path + test_file1}',
    f'-lh {regex1} {test_path + test_file0} {test_path + test_file1}',
    f'-ls {regex1} {test_path + test_file0} {test_path + test_file1} {fake_file}',
    f'-lf {test_path + regex_file} {test_path + test_file0} {test_path + test_file1}',
    f'-lo {regex1} {test_path + test_file0} {test_path + test_file1}',
    f'-nh {regex1} {test_path + test_file0} {test_path + test_file2}',
    f'-ns {regex1} {test_path + test_file0} {test_path + fake_file}',
    f'-nf {test_path + regex_file} {test_path + test_file0}',
    f'-no {regex1} {test_path + test_file0}',
    f'-hs {regex1} {test_path + test_file0} {test_path + test_file2}',
    f'-hf {test_path + regex_file} {test_path + test_file0} {test_path + test_file2}',
    f'-ho {regex1} {test_path + test_file0} {test_file2}',
    f'-sf {test_path + regex_file} {test_path + test_file0} {test_path + fake_file}',
    f'-so {regex1} {test_path + test_file0} {test_path + fake_file}',
    f'-of {test_path + regex_file} {test_path + test_file0}'
]

def compare(s21, original):
    with open(s21) as s21_grep_file:
        with open(original) as grep_file:
            s21, original = s21_grep_file.read(), grep_file.read()
            if s21 != original:
                print("\t\033[31mFAIL\033[37m")
            else:
                print("\t\033[32mSUCCESS\033[37m")

for index in range(len(flags)):
    print(f'\033[36mTEST \033[33m{index + 1}', end='')
    system(f'{s21_grep} {flags[index]} > {s21_res}')
    system(f'{grep} {flags[index]} > {original_res}')
    compare(s21_res, original_res)