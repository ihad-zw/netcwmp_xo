/**************************************************************************
  * Description
  *   This file implement all functions about link list operations.
  *
  * Remarks
  *   At current we implement double direction link operations.
  *
  * Bugs
  *   None.
  *
  * TODO
  *     * None.
  * History
  *   <table>
  *   \Author     Date        Change Description
  *   ----------  ----------  -------------------
  *   </table>
  *
  **************************************************************************/


#include "list.h"


/****************************************************************************
  * Description
  *   Insert a new entry between two known consecutive entries.
  *   This is only for internal list manipulation where we know the prev/next
  *   entries already!
  * Parameters
  *   entry :  New list entry.
  *   prev:  The prev entry.
  *   next:  The next entry.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   None.
  ****************************************************************************/
static inline void __list_add(struct list_head *entry, struct list_head * prev,
	struct list_head * next)
{
	next->prev = entry;
	entry->next = next;
	entry->prev = prev;
	prev->next = entry;
}


/****************************************************************************
  * Description
  *   Insert a new entry after the specified head.
  *   This is good for implementing stacks.
  * Parameters
  *   entry :  New entry to be added.
  *   head:  The list head entry to add it after.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   None.
  ****************************************************************************/
inline void list_add(struct list_head *entry, struct list_head *head)
{
	__list_add(entry, head, head->next);
}


/****************************************************************************
  * Description
  *   Insert a new entry before the specified head.
  *   This is useful for implementing queues.
  * Parameters
  *   entry :  New entry to be added.
  *   head:  The list head entry to add it before.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   None.
  ****************************************************************************/
inline void list_add_tail(struct list_head *entry, struct list_head *head)
{
	__list_add(entry, head->prev, head);
}


/****************************************************************************
  * Description
  *   Delete a list entry by making the prev/next entries point to each other.
  *   This is only for internal list manipulation where we know the prev/next
  *   entries already!
  * Parameters
  *   prev :  The deleted entry's prev entry.
  *   next :  The deleted entry's next entry.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   None.
  ****************************************************************************/
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}


/****************************************************************************
  * Description
  *   Deletes entry from list.
  * Parameters
  *   entry : The element to delete from the list.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   list_empty on entry does not return true after this, the entry is in an
  *   undefined state.
  ****************************************************************************/
inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}


/****************************************************************************
  * Description
  *   Deletes entry from list and reinitialize it.
  * Parameters
  *   entry : The element to delete from the list.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   None.
  ****************************************************************************/
inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}


/****************************************************************************
  * Description
  *   Tests whether a list is empty.
  * Parameters
  *   entry : The list to test.
  * Returns
  *   List empty or not.
  *
  * Return Value List
  *   0 : List is empty.
  *   Other value : List isn't empty.
  * Remarks
  *   None.
  ****************************************************************************/
inline int list_empty(struct list_head *head)
{
	return head->next == head;
}


/****************************************************************************
  * Description
  *   Join two lists
  * Parameters
  *   list : The new list to add.
  *   head : The place to add it in the first list.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   None.
  ****************************************************************************/
inline void list_splice(struct list_head *list, struct list_head *head)
{
	struct list_head *first = list->next;

	if (first != list) {
		struct list_head *last = list->prev;
		struct list_head *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}


/****************************************************************************
  * Description
  *   replace old entry by new one
  * Parameters
  *   old : the element to be replaced.
  *   new : the new element to insert.
  * Returns
  *   None.
  *
  * Return Value List
  *   None.
  * Remarks
  *   if 'old' was empty, it will be overwritten..
  ****************************************************************************/
inline void list_replace(struct list_head *old, struct list_head *new_list)
{
	new_list->next = old->next;
	new_list->next->prev = new_list;
	new_list->prev = old->prev;
	new_list->prev->next = new_list;
}

inline void list_replace_init(struct list_head *old, struct list_head *new_list)
{
	list_replace(old, new_list);
	INIT_LIST_HEAD(old);
}
