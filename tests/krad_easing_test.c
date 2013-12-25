#include <krad_easing.h>




static void easing_tests () {

  int position;
  int time;
  int start;
  int end;
  int duration;

  time = 1;
  position = 0;
  start = 0;
  end = 500;
  duration = 2;

  while (time <= duration) {

    position = kr_ease (LINEAR, time, start, end, duration);

    printf ("LINEAR Time: %d/%d Position is: %d/%d/%d\n", time, duration, start, position, end );

    time++;

  }

  time = 1;
  position = 0;
  start = 0;
  end = 500;
  duration = 3;

  while (time <= duration) {

    position = kr_ease (EASEINSINE, time, start, end, duration);

    printf ("EASEINSINE Time: %d/%d Position is: %d/%d/%d\n", time, duration, start, position, end );

    time++;

  }

  time = 1;
  position = 0;
  start = 0;
  end = 500;
  duration = 4;


  while (time <= duration) {

    position = kr_ease (EASEOUTSINE, time, start, end, duration);

    printf ("EASEOUTSINE Time: %d/%d Position is: %d/%d/%d\n", time, duration, start, position, end );

    time++;

  }


  time = 1;
  position = 0;
  start = 0;
  end = 500;
  duration = 5;

  while (time <= duration) {

    position = kr_ease (EASEINOUTSINE, time, start, end, duration);

    printf ("EASEINOUTSINE Time: %d/%d Position is: %d/%d/%d\n", time, duration, start, position, end );

    time++;

  }



  time = 1;
  position = 0;
  start = 0;
  end = 500;
  duration = 10;

  while (time <= duration) {

    position = kr_ease (EASEINCUBIC, time, start, end, duration);

    printf ("EASEINCUBIC Time: %d/%d Position is: %d/%d/%d\n", time, duration, start, position, end );

    time++;

  }

  time = 0;
  position = 0;
  start = 0;
  end = 500;
  duration = 10;


  while (time <= duration) {

    position = kr_ease (EASEOUTCUBIC, time, start, end, duration);

    printf ("EASEOUTCUBIC Time: %d/%d Position is: %d/%d/%d\n", time, duration, start, position, end );

    time++;

  }


  time = 0;
  position = 0;
  start = 0;
  end = -500;
  duration = 5;

  while (time <= duration) {

    position = kr_ease (EASEINOUTCUBIC, time, start, end, duration);

    printf ("EASEINOUTCUBIC Time: %d/%d Position is: %d/%d/%d\n", time, duration, start, position, end );

    time++;

  }

}

int main ( int argc, char *argv[] ) {


  kr_easer *easer;

  easer = kr_easer_create();
  kr_easer_destroy(easer);



  easing_tests ();


  return 0;

}
