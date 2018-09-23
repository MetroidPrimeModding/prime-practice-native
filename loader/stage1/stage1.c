typedef short fb_t;

extern void OSResetSystem(int);
extern void OSInit();
extern void VISetNextFrameBuffer(void*);
extern void VISetBlack(int);
extern void VIFlush();
extern void VIInit();

fb_t *fb1 = (fb_t*)0x804f0c00;
fb_t *fb2 = (fb_t*)0x80586c00;


void _payload() {
  *((int*)0x8045D850) = 0xFFFFFFFF;

  int fb = 1;

  for (int i = 0; i < 240 * 640; i++) {
    fb1[i] = (fb_t)0xDDDD;
    fb2[i] = (fb_t)0x3333;
  }

  OSResetSystem(2);
  OSInit();
  VIInit();
  VISetBlack(0);
  while (1) {
    if (fb == 1) {
      VISetNextFrameBuffer(fb2);
      fb = 2;
    } else {
      VISetNextFrameBuffer(fb1);
      fb = 1;
    }
    VIFlush();
  }


  //hang
//  while (1) {};
  asm("b return_to_smash");
}