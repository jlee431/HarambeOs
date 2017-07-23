#include "keyboard.h"
#include "rtc.h"
#include "kernel.h"
#include "system_calls.h"
#include "paging.h"
#include "lib.h"

/*taken from http://www.osdever.net/bkerndev/Docs/keyboard.htm */

/* declare and initialize the keyboard map */

int caps_flag = OFF;                    /* caps lock flag */
int right_shift_flag = OFF;             /* right shift flag */
int left_shift_flag = OFF;              /* left shift flag */
int control_flag = OFF;                 /* to tell if control is pressed */
int ent_flag[TERMINAL_WINDOWS] = {0};                     /* is enter pressed */
int inc_or_dec = 0;                     /* did we increment or decrement? */
int ctrl_4_flag = OFF;                  /* testing flag for cotrol 4 */
int alt_flag = 0;                       /* this is used for the alt condition */
int putc_flag = 0;

/* declare the multiple keyboard buffers here */
unsigned char buffer[TERMINAL_WINDOWS][NUMBER_CODES] = {{'\0'}};      /* character buffer for terminal */
unsigned char read[NUMBER_CODES] = {'\0'};

/* declare the muliple terminal buffers here */
unsigned char terminal_buffer[TERMINAL_WINDOWS][WINDOW_SIZE] = {{'\0'}};

/* mappings of the vodeo for each terminal */
unsigned char *term_buf_0 = (unsigned char *) FIRST_TERM_VID;
unsigned char *term_buf_1  =(unsigned char *) SECOND_TERM_VID;
unsigned char *term_buf_2 = (unsigned char *) THIRD_TERM_VID;

/* the varaible lets us know the current terminal, 0, 1, or 2 */
int current_terminal = 0;
int active_terminal = 0;

int line_count[TERMINAL_WINDOWS] = {0};

/* two flags */
int term_2_flag = 0;
int term_3_flag = 0;

int other_2_shells = 0;

char * message = "too many processes\n";

int rtc_test = 2;                       /* test frequency */

int i;                                  /* test control 2 */
int k;                                  /* test control 3 */

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
unsigned char kbdus[NUMBER_CODES] =
{
    0,  0, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', 0,	/* Backspace */
  0,			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  0,
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  0,
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  0,
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/* an array to store caps lock only */
unsigned char caps[NUMBER_CODES] =
{
    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', 0, /* Backspace */
  0,     /* Tab */
  'Q', 'W', 'E', 'R', /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', /* Enter key */
    0,      /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', /* 39 */
 '\'', '`',   0,    /* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
  'M', ',', '.', '/',   0,        /* Right shift */
  0,
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  0,
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  0,
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/* an array of shift without caps */
unsigned char shift_no_caps[NUMBER_CODES] =
{
    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', 0, /* Backspace */
  0,     /* Tab */
  'Q', 'W', 'E', 'R', /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,      /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
 '\"', '~',   0,    /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
  'M', '<', '>', '?',   0,        /* Right shift */
  0,
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  0,
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  0,
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/* an array of shift with caps */
unsigned char shift_and_caps[NUMBER_CODES] =
{
    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', 0, /* Backspace */
  0,     /* Tab */
  'q', 'w', 'e', 'r', /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', /* Enter key */
    0,      /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', /* 39 */
 '\"', '~',   0,    /* Left shift */
 '|', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
  'm', '<', '>', '?',   0,        /* Right shift */
  0,
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  0,
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  0,
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/*
 * keyboard_initialization
 *   DESCRIPTION: function initializes the keyboard by connecting it to the PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: connects keyboard to PIC
 */

void keyboard_intialization(void)
{
  /* connect keyboard to the PIC */
  enable_irq(1);
}


/*
 *   keyboard
 *   DESCRIPTION: keyboard handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: echos the character onto the screen for checkpoint 1
 */

void keyboard(void)
{
    putc_flag = 0;
  send_eoi(KEYBOARD_NUM);
  /* set the cursor position */
  cursor_wrapper(1);
  int j = 0;                  /* loop through the buffer */
  int screen = 0;
  unsigned char print;        /* character to be echoed */

  /* check the status port */
  if(inb(KBD_STATUS_PORT) != 0)
  {
    unsigned char scan = inb(KBD_DATA_PORT);		         /* obtain the keyboard input  */

    /* check if any special key has been pressed */
    if(scan >= RELEASE_START || scan == CAPS_CODE || scan == RIGHT_SHIFT_PRESSED || scan == LEFT_SHIFT_PRESSED || scan == CTRL || scan == ALT)
    {
      /* ignore keyboard release scan codes */
      /* set the control flag */
      if(scan == CTRL)
        control_flag = ON;
      else if(scan == CTRL_RELEASE)
        control_flag = OFF;

      /* set the caps flag */
     if(scan == CAPS_CODE)
        caps_flag = !caps_flag;

      /* right shift */
      if(scan == RIGHT_SHIFT_PRESSED)
       right_shift_flag = ON;
      else if(scan == RIGHT_SHIFT_RELEASED)
        right_shift_flag = OFF;

     /* left shift */
      if(scan == LEFT_SHIFT_PRESSED)
        left_shift_flag = !left_shift_flag;
      else if(scan == LEFT_SHIFT_RELEASED)
        left_shift_flag = !left_shift_flag;

      /* ALT */
      if(scan == ALT)
        alt_flag = ON;
      else if(scan == ALT_RELEASE)
        alt_flag = OFF;

      return;
    }

    /* check if control has been pressed */
    if(control_flag == ON && scan == L)
    {
      /* clear the screen, write the buffer to terminal, and set the cursor */
      clear();
      control_l(current_terminal, (const char **)buffer, line_count[current_terminal]);
      set_scroll(line_count[current_terminal]);
      cursor_wrapper(1);
      return;
    }

    /* control 1 test case */
    else if(control_flag == ON && scan == ONE)
    {
      clear();
      control_1();
      return;
    }

    /* control 2 test case */
    else if(control_flag == ON && scan == TWO)
    {
      clear();
      int total;
      total = i % TEST_CONTROL;
      control_2(total);
      i++;
      return;
    }

    /* control 3 test case */
    else if(control_flag == ON && scan == THREE)
    {
      clear();
      int total;
      total = k % TEST_CONTROL;
      control_3(total);
      k++;
      return;
    }

    /* control 4 test case */
    else if(control_flag == ON && scan == FOUR)
    {

      clear();

      /* flush the buffer */
      for(j = 0; j < NUMBER_CODES; j++)
        read[j] = '\0';

      /* test the rtc write function */
      ctrl_4_flag = ON;
      rtc_write(0, &rtc_test, FOUR_BYTE_MUST);
      rtc_test *= DOUBLE;

      /* test rtc read and and terminal write */
      while(ctrl_4_flag)
        control_4(line_count[current_terminal], rtc_test);

      set_scroll(line_count[current_terminal]);

      cursor_wrapper(1);
      return;
    }

    /* control 5 test case */
    else if(control_flag == ON && scan == FIVE)
    {
      int j = 0;              /* loop through the entire buffer */
      clear();
      reset_cursor();

      /* flush keyboard buffer */
      for(j = 0; j < NUMBER_CODES; j++)
        buffer[current_terminal][j] = '\0';

      /* test rtc write by resetting frequency to 2 hz */
      ctrl_4_flag = OFF;
      rtc_test = DOUBLE;

      /* test rtc wrote */
      control_5(rtc_test);
      return;
    }

    /* alt f1 case */
    if(alt_flag == ON && scan == F1)
    {
        /* switch to first terminal */
        cli();
        first_terminal_switch();
        sti();
        return;
    }

    /* alt f2 case */
    else if(alt_flag == ON && scan == F2)
    {
        /* switch to second terminal */
        cli();
        second_terminal_switch();
        sti();
        return;
    }

    /* alt f3 case */
    else if(alt_flag == ON && scan == F3)
    {
        /* switch to third terminal */
        cli();
        third_terminal_switch();
        sti();
        return;
    }


    /* now that the flags have been set, determine which array to use */
    if(caps_flag == OFF && right_shift_flag == OFF && left_shift_flag == OFF)
      print = kbdus[scan];

    else if(caps_flag == OFF && right_shift_flag == OFF && left_shift_flag == ON)
      print = shift_no_caps[scan];

    else if(caps_flag == OFF && right_shift_flag == ON && left_shift_flag == OFF)
      print = shift_no_caps[scan];

    else if(caps_flag == OFF && right_shift_flag == ON && left_shift_flag == ON)
      print = shift_no_caps[scan];

    else if(caps_flag == ON && right_shift_flag == OFF && left_shift_flag == OFF)
      print = caps[scan];

    else if(caps_flag == ON && right_shift_flag == OFF && left_shift_flag == ON)
      print = shift_and_caps[scan];

    else if(caps_flag == ON && right_shift_flag == ON && left_shift_flag == OFF)
      print = shift_and_caps[scan];

    else if(caps_flag == ON && right_shift_flag == ON && left_shift_flag == ON)
      print = shift_and_caps[scan];

    /* check if scrolling is necessary */
    if(check_screen(scan, line_count[current_terminal], inc_or_dec))
        scroll_2();

    /* check if backspace is needed */
    if(scan == BACKSPACE)
    {
        if(line_count[current_terminal] > 0)
        {
          /* modify the buffer ad animate the backspace */
          line_count[current_terminal]--;
          buffer[current_terminal][line_count[current_terminal]] = '\0';
          inc_or_dec = 0;     /* set the scrolling flag */
          backspace();
        }

        return;
    }

    /* dont put in any tab, enter or backspace into buffer */
    /* tab doesnt get echoed, backspace should be echoed, enter should be echoded */
    if(print != 0)
    {
      if(scan != ENTER)
      {
        if(line_count[current_terminal] < NUMBER_CODES)
        {
          /* get the screen position to see if newline ins necessary */
          screen = get_screen_x(1);

          if(screen == 0 && line_count[current_terminal] > 0)
            putc_number_2('\n');

          /* echo character and fill in the buffer */
          inc_or_dec = 1;               /* set the scrolling flag */
          putc_flag = 1;

            putc_number_2(print);
            buffer[current_terminal][line_count[current_terminal]] = print;
            line_count[current_terminal]++;
        }
        return;
      }

      /* enter has been pressed */
      else
      {
        ent_flag[current_terminal] = ON;               /* the current terminal is the viewing terminal */

        /* clear the buffer */
        for(j = 0; j < NUMBER_CODES; j++)
          read[j] = '\0';

        /* fill in the read buffer */
        for(j = 0; j < NUMBER_CODES; j++)
          read[j] =  buffer[current_terminal][j];

        /* flush keyboard buffer */
        for(j = 0; j < NUMBER_CODES; j++)
          buffer[current_terminal][j] = '\0';

        /* echo and reset the indicies */
        putc_number_2('\n');
        line_count[current_terminal] = 0;

        return;
      }
    }
  }
}

/*
 *   terminal_read
 *   DESCRIPTION: reads data from the keyboard
 *   INPUTS: fd -- not used for checkpoint 2
 *       buf -- pointer to buffer
 *       nbytes -- number of bytes
 *   RETURN VALUE: returns the data from one line that has terminated when enter is pressed
 */

int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes)
{

  unsigned char *ptr = (unsigned char*) buf;  /* pointer to the buffer */

  if(ptr == NULL)
    return -1;

  int j = 0;                                  /* loop through the buffer */

  /* loop stops when enter is pressed */
  while(1)
  {
    if(ent_flag[active_terminal] == ON)       /* active terminal is the scheduled one */
    {
      ent_flag[active_terminal] = OFF;
      break;
    }
  }

  /* flush the buffer */
  for(j = 0; j < BUF_SIZE; j++)
    ptr[j] = '\0';

  /* copy over the data */
  for (j = 0; j < nbytes; j++)
  {
    ptr[j] = read[j];
  }

  /* flush the buffer */
  for(j = 0; j < NUMBER_CODES; j++)
    read[j] = '\0';

  /* return the number of bytes */
  return j;
}

/*
 *   terminal_write
 *   DESCRIPTION: writes data to the terminal
 *   INPUTS: fd -- file descriptor
 *       buf -- pointer to buffer
 *       nbytes -- number of bytes
 *   RETURN VALUE: 0
 */

int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes)
{

    cli();
    unsigned char *ptr = (unsigned char*) buf;   /* a pointer to our buffer */

    int j = 0;                                   /* loop through the buffer */
    int i = 0;
    int screen_x_temp = 0;
    int screen_y_temp = 0;

     /* standard error checking */
    if(ptr == NULL)
        return ERROR;

    /* Replace all null char w/ space */
    for(i=0; i<nbytes; i++)
    {
        if (ptr[i] == '\0')
        ptr[i] = 0x20;      /* space character */
    }

    /* display all the data on the screen */
    while(j < nbytes)
    {
        /* get x and y */
        screen_x_temp = get_screen_x(0);
        screen_y_temp = get_screen_y(0);

        if(screen_x_temp == MAX_X && screen_y_temp == MAX_Y && ptr[j] != '\n' && ptr[j] != '\0')
        {
            /* output the character ad then scroll */
            putc(ptr[j]);
            scroll();
            putc('\n');
            j++;
        }

        else if(screen_x_temp == MAX_X && screen_y_temp == MAX_Y && ptr[j] == '\n')
        {
            /* scroll and output the enter key */
            scroll();
            putc(ptr[j]);
            j++;
        }

        else if(screen_y_temp == MAX_Y && ptr[j] == '\n')
        {
            /* scroll and then output the enter key */
            scroll();
            putc(ptr[j]);
            j++;
        }

        else if(screen_x_temp == MAX_X && ptr[j] == '\n')
        {
            /* output the enter key */
            putc(ptr[j]);
            j++;
        }

        else if(screen_x_temp == MAX_X)
        {
            /* output the character, and then enter */
            putc(ptr[j]);
            putc('\n');
            j++;
        }

        else if(ptr[j] == '\n')
        {
            /* output the eter key */
            putc('\n');
            j++;
        }

        else if(ptr[j] == '\0')
        {
            /* finish if null */
            break;
        }

        else
        {
            /* print a normal character */
            putc(ptr[j]);
            j++;
        }
    }
    sti();                  /* set the cursor */
    cursor_wrapper(0);
    return j;
}

/*
 *   terminal_open
 *   DESCRIPTION: not defined for checkpoint 2
 *   INPUTS: fd -- file descriptor
 *       buf -- pointer to buffer
 *       nbytes -- number of bytes
 *   RETURN VALUE: 0
 */

int32_t terminal_open(const uint8_t* filename)
{
  /* undefined for terminal */
  return 0;
}

/*
 *   terminal_close
 *   DESCRIPTION: not defined for checkpoint 2
 *   INPUTS: fd -- file descriptor
 *   RETURN VALUE: 0
 */

int32_t terminal_close(int32_t fd)
{
  /* undefined for terminal */
  return 0;
}


/*
    stdin_write
    DESCRIPTION: stdin doesn't write anything,so it just retuns -1
    INPUTS: NONE
    RETURN VALUE: -1
*/
int32_t stdin_write(){
  return -1;
}

/*
    stdout_read
    DESCRIPTION: stdout doesn't read anything,so it just retuns -1
    INPUTS: NONE
    RETURN VALUE: -1
*/
int32_t stdout_read(){
  return -1;
}


/*
    remove_exit
    DESCRIPTION: checks if exit and removes enter key
    INPUTS: buf -- a pointer to the buffer
    RETURN VALUE: none
*/

void remove_exit(char *buf)
{
  int i = 0;                        /* loop through th buffer */
  int flag = OFF;                   /* break from loop */
  const char *compare = "exit";     /* comparison string */

  /* compare the strings */
  for(i = 0; i < EXIT_LIMIT; i++)
  {
    if(compare[i] != buf[i])
      flag = ON;
  }

  /* remove the enter character */
  if(flag == OFF)
    buf[EXIT_LIMIT] = '\0';

}


/*
    remove_hello
    DESCRIPTION: checks if hello and removes enter key
    INPUTS: buf -- a pointer to the buffer
    RETURN VALUE:  none
*/

void remove_hello(char *buf)
{
  int i = 0;                        /* loop through th buffer */
  int flag = OFF;                   /* break from loop */
  const char *compare = "hello";    /* comparison string */

  /* compare the strings */
  for(i = 0; i < HELLO_LIMIT; i++)
  {
    if(compare[i] != buf[i])
      flag = ON;
  }

  /* remove the enter character */
  if(flag == OFF)
    buf[HELLO_LIMIT] = '\0';

}


/*
    remove_testprint
    DESCRIPTION: checks if testprint and removes enter key
    INPUTS:buf -- a pointer to the buffer
    RETURN VALUE: none
*/

void remove_testprint(char *buf)
{
    int i = 0;                      /* loop through th buffer */
    int flag = OFF;                 /* break from loop */
    char *compare = "testprint";    /* comparison string */

    /* compare the strings */
    for(i = 0; i < TESTPRINT_LIMIT; i++)
    {
        if(compare[i] != buf[i])
            flag = ON;
    }

    /* remove the enter character */
    if(flag == OFF)
        buf[TESTPRINT_LIMIT] = '\0';

}


/*
    first_terminal_switch
    DESCRIPTION: function swtiches to the first terminal and sets it up as necessary
    INPUTS: none
    RETURN VALUE: none
*/

void first_terminal_switch(void)
{

    remap_term_before(current_terminal);

    if(current_terminal == 0)
        memcpy((void*)term_buf_0, (const void*) VIDEO, WINDOW_SIZE);

    else if(current_terminal == 1)
        memcpy((void*)term_buf_1, (const void*) VIDEO, WINDOW_SIZE);

    else if(current_terminal == 2)
        memcpy((void*)term_buf_2, (const void*) VIDEO, WINDOW_SIZE);


    /* make sure to update the current terminal, now we can do the next step */
    current_terminal = 0;

    if(current_terminal == 0)
        memcpy((void*)VIDEO, (const void*) term_buf_0, WINDOW_SIZE);

    else if(current_terminal == 1)
        memcpy((void*)VIDEO, (const void*) term_buf_1, WINDOW_SIZE);

    else if(current_terminal == 2)
        memcpy((void*)VIDEO, (const void*) term_buf_2, WINDOW_SIZE);

    remap_term(current_terminal);

return;
}


/*
    second_terminal_switch
    DESCRIPTION: function swtiches to the second terminal and sets it up as necessary
    INPUTS: none
    RETURN VALUE: none
*/

void second_terminal_switch(void)
{

    if(universal_count() == 0 && term_2_flag == 0)
    {
        terminal_write(0, message, strlen(message));
        return;
    }

    remap_term_before(current_terminal);  // Change the addr that putc uses... video mem to buffer

    if(current_terminal == 0)
        memcpy((void*)term_buf_0, (const void*) VIDEO, WINDOW_SIZE);

    else if(current_terminal == 1)
        memcpy((void*)term_buf_1, (const void*) VIDEO, WINDOW_SIZE);

    else if(current_terminal == 2)
        memcpy((void*)term_buf_2, (const void*) VIDEO, WINDOW_SIZE);

    /* make sure to update the current terminal, now we can do the next step */
    current_terminal = 1;

    if(current_terminal == 0)
        memcpy((void*)VIDEO, (const void*) term_buf_0, WINDOW_SIZE);

    else if(current_terminal == 1)
        memcpy((void*)VIDEO, (const void*) term_buf_1, WINDOW_SIZE);

    else if(current_terminal == 2)
        memcpy((void*)VIDEO, (const void*) term_buf_2, WINDOW_SIZE);

    remap_term(current_terminal);

    /* first time user makes the switch ?*/
    if(term_2_flag == 0)
    {
        term_2_flag = 1;
        reset_cursor();
        active_terminal = current_terminal;
        c_execute((uint8_t*)"shell");
    }

    return;
}


/*
    third_terminal_switch
    DESCRIPTION: function swtiches to the third terminal and sets it up as necessary
    INPUTS: none
    RETURN VALUE: none
*/

void third_terminal_switch(void)
{

    if(universal_count() == 0 && term_3_flag == 0)
    {
        terminal_write(0, message, strlen(message));
        return;
    }

    remap_term_before(current_terminal);

    if(current_terminal == 0)
        memcpy((void*)term_buf_0, (const void*) VIDEO, WINDOW_SIZE);

    else if(current_terminal == 1)
        memcpy((void*)term_buf_1, (const void*) VIDEO, WINDOW_SIZE);

    else if(current_terminal == 2)
        memcpy((void*)term_buf_2, (const void*) VIDEO, WINDOW_SIZE);

    /* make sure to update the current terminal, now we can do the next step */
    current_terminal = 2;

    if(current_terminal == 0)
        memcpy((void*)VIDEO, (const void*) term_buf_0, WINDOW_SIZE);

    else if(current_terminal == 1)
        memcpy((void*)VIDEO, (const void*) term_buf_1, WINDOW_SIZE);

    else if(current_terminal == 2)
        memcpy((void*)VIDEO, (const void*) term_buf_2, WINDOW_SIZE);

    remap_term(current_terminal);


    if(term_3_flag == 0)
    {
        /* call the execute */
        term_3_flag = 1;
        reset_cursor();
        active_terminal = current_terminal;
        c_execute((uint8_t*)"shell");
    }

    return;
}

/*
    get_active_term
    DESCRIPTION: function returns the active terminal
    INPUTS: none
    RETURN VALUE: none
*/

int get_active_term(void)
{
  return active_terminal;
}


/*
    change_active_term
    DESCRIPTION: function changes the active terminal
    INPUTS: temp_term -- the terminal representation passed in
    RETURN VALUE: none
*/

void change_active_term(int temp_term)
{
    active_terminal = temp_term;
}


/*
    get_current_term
    DESCRIPTION: function returns the current (viewing) terminal
    INPUTS: none
    RETURN VALUE: none
*/

int get_current_term(void)
{
    return current_terminal;
}
