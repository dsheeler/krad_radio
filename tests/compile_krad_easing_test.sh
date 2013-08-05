gcc -g -Wall -I../lib/krad_chronometer -I../lib/krad_compositor/ -I../tools/krad_system/ ../lib/krad_chronometer/krad_easing.c \
../lib/krad_system/krad_system.c krad_easing_test.c -o krad_easing_test \
-pthread -lm -DKR_LINUX
