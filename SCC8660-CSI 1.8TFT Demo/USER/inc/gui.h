#ifndef GUI_H_
#define GUI_H_
#include "headfile.h"

#define CSI_IMAGE 0
#define BIN_IMAGE 1
#define LINE      3
#define SRC_IMAGE 2


#define  KEY_UP		1       /*上 */
#define  KEY_DOWN	2       /*下 */
#define  KEY_LEFT	3       /* 左 */
#define  KEY_RIGHT	4       /* 右 */
#define  KEY_MID	5       /*选择 */
#define pageMax		5       /* 最大页数 */

extern int			keyState;
extern int			menuRow;
extern int			temp1;
extern int			temp2;
extern int			temp3;
extern int			temp4;
extern int			temp5;
extern int			temp6;
extern int			temp7;
extern int			key_counter;
extern int			key_temp;
extern const unsigned char	row_pos[7];
extern int clearCount;
extern int start;
extern void menu();

extern void flashWrite();
extern void SignMove();


extern int keyCheck( void );


extern void FlashValueOperate();


extern void MenuShow3();


extern void MenuShow2();

extern void flashDataSave();
extern void flashDataRead();
extern void show_line();
extern void init_Key();

extern int straight_threshold;




#endif

