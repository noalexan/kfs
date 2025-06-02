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

### Multiple Zones and Migration Type handling

- Be carefull this idea need to chage lots of code like boot allocator implementation page descriptor and maybe some lines on buddy allocator =).

-   Refactor bootallocator like this to hndle multiple zone :

```c
typedef struct boot_allocator {
    bool state;
    zone_regions_t zones[ZONE_TYPE_COUNT][REGIONS_TYPES];
} boot_allocator_t;

```

- on page descriptor parsing use an metrics descriptor to segment free regions into page with an MIGRATION TYPE flags following metrics :

```c
typedef struct migration_metrics_s {
    int proportions;
    size_t min_size;
    bool allow_fallback[MIGRATION_TYPE_COUNT];
} migration_metrics_t;

migration_metrics_t metrics[MIGRATION_TYPE_COUNT];
```

- Maybe an refactoring of buddy allocator impl is needed.

## Test Framework / Driver

## TSS Maybe on KFS 5

Follow Os dev to implement TSS
Stack Allocator
