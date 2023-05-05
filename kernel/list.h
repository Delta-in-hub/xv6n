/*
根据linux kernel list ,自己实现的侵入式双向链表,实现了大部分功能
内核/通用链表其实就是一个有头节点的双向循环链表
侵入式的设计使得链表可以把不同数据类型串起来,突破了传统链表仅可以储存相同的数据类型的限制
*/

#ifndef KERNELLIST_H
#define KERNELLIST_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef size_t
typedef unsigned long size_t;
#endif

// struct type中成员name的地址偏移量
#define offsetof(type, name) ((size_t) & ((type *)0)->name)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 * 得到ptr所指向的结构体的起始地址
 * typeof为gnu拓展,省去仅少了类型检查,不影响正常实现
 */
#define container_of(ptr, type, name)                                          \
  (type *)((char *)ptr - offsetof(type, name))

// 得到ptr所指向的类型为type的结构体的起始地址, name为结构体中list_entry的变量名
#define listEntry(ptr, type, name) (container_of(ptr, type, name))

/**
 * traverse the whole list, p is a list_entry * as a loop cursor
 * @p:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define traverse(p, head)                                                      \
  for (list_entry *p = (head)->succ, *list_entry_temp = p->succ; p != (head);  \
       p = list_entry_temp, list_entry_temp = p->succ)

/**
 * Reversed traverse the whole list
 * @p:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define traverseReverse(p, head)                                               \
  for (list_entry *p = (head)->prev, *list_entry_temp = p->prev; p != (head);  \
       p = list_entry_temp, list_entry_temp = p->prev)

/**
 * From this entry to traverse the remain list and end at the head.
 * @p:	the &struct list_head to use as a loop cursor.
 * @entry:  A entry of the list.
 * @head:	the head for your list.
 */
#define traverseFromEntry(p, entry, head)                                      \
  for (list_entry *p = entry->succ, *list_entry_temp = p->succ; p != (head);   \
       p = list_entry_temp, list_entry_temp = p->succ)

/**
 * From this entry to reversed traverse the remain list and end at the head.
 * @p:	the &struct list_head to use as a loop cursor.
 * @entry:  A entry of the list.
 * @head:	the head for your list.
 */
#define traverseReverseFromEntry(p, entry, head)                               \
  for (list_entry *p = entry->prev, *list_entry_temp = p->prev; p != (head);   \
       p = list_entry_temp, list_entry_temp = p->prev)

typedef struct list_entry {
  struct list_entry *prev, *succ;
} list_entry;

static void list_entry_init(list_entry *p) {
  p->prev = p;
  p->succ = p;
}

// Whether entry is the last entry in list head
static int isLast(list_entry *entry, list_entry *head) {
  return entry->succ == head && head->prev == entry;
}

/*
 * 内核链表不管理内存,插入节点时插入对象由使用者开辟空间,删除节点时也不会释放空间
 * 链表仅仅将它们串起来,可以通过这条链访问
 */

// data will be the first element in the head's list,
// insert after the head
static void pushFront(list_entry *data, list_entry *head) {
  // 将data插在head与head->succ之间
  data->prev = head;
  data->succ = head->succ;
  head->succ->prev = data;
  head->succ = data;
}

// data will be the last element in the head's list,
// insert before the head
static void pushBack(list_entry *data, list_entry *head) {
  pushFront(data, head->prev);
}

/**
 * deletes entry from list.
 * @entry: the element to delete from the list.
 */
static void delEntry(list_entry *entry) {
  entry->prev->succ = entry->succ;
  entry->succ->prev = entry->prev;
  entry->prev = NULL;
  entry->succ = NULL;
}

#endif
