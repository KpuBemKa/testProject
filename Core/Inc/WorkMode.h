#pragma once

/**
 * 0. TempOpenMode
 * 1. DenyMode
 * 2. TempAlarmMode
 * 3. AlarmMode
 * 4. NoMode
*/
enum class WorkMode
{
  GrantAccessMode,
  PassageMode,
  DenyMode,
  TempAlarmMode,
  AlarmMode,
  NoMode
};