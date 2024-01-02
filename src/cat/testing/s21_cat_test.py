from itertools import combinations_with_replacement
from random import shuffle
from os import system

s21_grep = "./s21_cat"
grep = "cat"

s21_res = 's21_cat.res'
original_res = 'cat.res'

test_path = 'testing/'
test_file0 = 'test0.txt'

flags = [
    f'-b {test_path + test_file0}',
    f'--number-nonblank {test_path + test_file0}',
    f'-e {test_path + test_file0}',
    f'-E {test_path + test_file0}',
    f'-n {test_path + test_file0}',
    f'--number {test_path + test_file0}',
    f'-s {test_path + test_file0} {test_path + test_file0}',
    f'--squeeze-blank {test_path + test_file0}',
    f'-t {test_path + test_file0}',
    f'-T {test_path + test_file0}'
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