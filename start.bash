#!/bin/bash

scp src/cat/s21_cat  student@172.24.116.8:/usr/local/bin/s21_cat
scp src/grep/s21_grep  student@172.24.116.8:/usr/local/bin/s21_grep
hostname; ls -la /usr/local/bin/
ssh student@172.24.116.8 'hostname; ls -la /usr/local/bin/'
