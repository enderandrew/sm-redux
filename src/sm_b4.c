// B4 - Enemy instructions, sets, drop chances, resistances (complete)
#include "ida_types.h"
#include "variables.h"
#include "funcs.h"
#include "enemy_types.h"


#define kCreateSprite_Ilists ((uint16*)RomFixedPtr(0xb4bda8))




uint16 CreateSpriteAtPos(uint16 x_r18, uint16 y_r20, uint16 ilist_r22, uint16 pal_r24) {  // 0xB4BC26
  int v0 = 62;
  while (sprite_instr_list_ptrs[v0 >> 1]) {
    v0 -= 2;
    if ((v0 & 0x8000) != 0)
      return 0xffff;
  }
  int v1 = v0 >> 1;
  sprite_palettes[v1] = 0;
  sprite_x_subpos[v1] = 0;
  sprite_y_subpos[v1] = 0;
  sprite_disable_flag[v1] = 0;
  sprite_x_pos[v1] = x_r18;
  sprite_y_pos[v1] = y_r20;
  sprite_palettes[v1] = pal_r24;
  uint16 v2 = kCreateSprite_Ilists[ilist_r22];
  sprite_instr_list_ptrs[v1] = v2;
  sprite_instr_timer[v1] = *(uint16 *)RomPtr_B4(v2);
//  R18 = v0;
  return v0;
}

void CallSpriteObjectInstr(uint32 ea) {
  switch (ea) {
  case fnSpriteObject_Instr_RepeatLast: SpriteObject_Instr_RepeatLast(); return;  // 0xb4bcf0
  case fnSpriteObject_Instr_Terminate: SpriteObject_Instr_Terminate(); return;  // 0xb4bd07
  case fnSpriteObject_Instr_Goto: SpriteObject_Instr_Goto(); return;  // 0xb4bd12
  default: Unreachable();
  }
}

void HandleSpriteObjects(void) {  // 0xB4BC82
  uint16 v1;

  if (debug_time_frozen_for_enemies | time_is_frozen_flag)
    return;

  sprite_object_index = 62;
  do {
    int v0;
    v0 = sprite_object_index >> 1;
    if (!sprite_instr_list_ptrs[v0] || (sprite_disable_flag[v0] & 1) != 0)
      continue;
    v1 = sprite_instr_timer[v0];
    if (sign16(v1)) {
BREAKLABEL:
      CallSpriteObjectInstr(v1 | 0xB40000);
      continue;
    }
    sprite_instr_timer[v0] = v1 - 1;
    if (v1 == 1) {
      uint16 v3 = sprite_instr_list_ptrs[v0] + 4;
      sprite_instr_list_ptrs[v0] = v3;
      v1 = *(uint16 *)RomPtr_B4(v3);
      if (sign16(v1))
        goto BREAKLABEL;
      sprite_instr_timer[sprite_object_index >> 1] = v1;
    }
  } while (!sign16(sprite_object_index -= 2));
}

void SpriteObject_Instr_RepeatLast(void) {  // 0xB4BCF0
  int v2 = sprite_object_index >> 1;
  sprite_instr_list_ptrs[v2] -= 4;
  sprite_instr_timer[v2] = 0x7FFF;
}

void SpriteObject_Instr_Terminate(void) {  // 0xB4BD07
  sprite_instr_list_ptrs[sprite_object_index >> 1] = 0;
}

void SpriteObject_Instr_Goto(void) {  // 0xB4BD12
  uint16 v2 = GET_WORD(RomPtr_B4(sprite_instr_list_ptrs[sprite_object_index >> 1]) + 2);
  sprite_instr_list_ptrs[sprite_object_index >> 1] = v2;
  sprite_instr_timer[sprite_object_index >> 1] = GET_WORD(RomPtr_B4(v2));
}

void DrawSpriteObjects(void) {  // 0xB4BD32
  for (int i = 62; i >= 0; i -= 2) {
    int v1 = i >> 1;
    if (sprite_instr_list_ptrs[v1]) {
      uint16 x = sprite_x_pos[v1] - layer1_x_pos;
      if ((int16)(x + 16) >= 0) {
        if (sign16(x - 272)) {
          int16 y = sprite_y_pos[v1] - layer1_y_pos;
          if (y >= 0) {
            if (sign16(y - 272)) {
              uint16 r3 = sprite_palettes[v1] & 0xE00;
              uint16 r0 = sprite_palettes[v1] & 0x1FF;
              const uint8 *v3 = RomPtr_B4(sprite_instr_list_ptrs[v1]);
              DrawSpritemapWithBaseTile(0xB4, GET_WORD(v3 + 2), x, y, r3, r0);
            }
          }
        }
      }
    }
  }
}

void ClearSpriteObjects(void) {  // 0xB4BD97
  int v0 = 1022;
  do {
    sprite_instr_list_ptrs[v0 >> 1] = 0;
    v0 -= 2;
  } while (v0 >= 0);
}
