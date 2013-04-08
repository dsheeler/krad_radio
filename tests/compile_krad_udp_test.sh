gcc -Wall krad_udp_test.c -DKR_LINUX -I../lib/krad_udp/ -I../lib/krad_vpx/\
-I../lib/krad_system/ \
../lib/krad_system/krad_system.c \
../lib/krad_vpx/krad_vpx.c \
../lib/krad_udp/krad_udp.c -o krad_udp_test \
-pthread -lm `pkg-config --libs --cflags vpx`
