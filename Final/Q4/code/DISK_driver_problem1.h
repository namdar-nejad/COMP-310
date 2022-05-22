//    Final Project COMP 310
//    Your Name: NAMDAR KABOLINEJAD
//    Your McGill ID: 260893536


/*
activeIndex: the index of the active_file_table to fill
name: name of the file to open

Returns:
the index of the fat table with the file name
-1 if the index number is out of bounds
-2 if the index is in use
-3 if there is no empty slot in the FAT Table
-4 on other errors
*/
int open_EXAM_function(int activeIndex, char *name);