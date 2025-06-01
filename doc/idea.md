# Improvement Idea

## Keyboard

[ ] refactor to use dynamic memory

### Input Driver

[ ] Create an input driver for follow this workflow :

```c
    // KEYBOARD driver -> INPUT driver + Line Editing <-> tty -> VGA driver 
```

## Memory

[~] Use a Buddy Allocator for the PMM
[ ] Create differente memory zone : DMA / DMA32 / NORMAL  Only if neccesary
[ ] Create differente MIGRATION_TYPE to optimize framgmentation handling

## Test Framework / Driver

## TSS Maybe on KFS 5

Follow Os dev to implement TSS
Stack Allocator
