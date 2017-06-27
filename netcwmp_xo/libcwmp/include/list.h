/**************************************************************************
  * Description
  *   This file declare all exported function, macro and variable about link
  *   list operations.
  *
  * Remarks
  *   At current we implement double direction link operations.
  *
  * Bugs
  *   \None.
  *
  * TODO
  *     *
  * History
  *   <table>
  *   \Author     Date         Change Description
  *   ----------  -----------  -------------------
  *   </table>
  *
  **************************************************************************/

#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__



#ifdef __GNUC__
#define prefetch(x)		hal_cache_prefetch_load(x)
#else
#define prefetch(x)		(x)
#endif
/* Double direction link structure */
struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD(name)	\
	struct list_head name = {&(name), &(name)}

#define INIT_LIST_HEAD(ptr)	\
	do { (ptr)->next = (ptr); (ptr)->prev = (ptr); } while (0)

/* list_get_entry - Get the struct for this entry.
 *  ptr:	the &struct list_head pointer.
 *  type:	the type of the struct this is embedded in.
 *  member:	the name of the list_struct within the struct.
 */
#define list_get_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/* list_for_each - Iterate over a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for(pos = (head)->next, prefetch(pos->next); pos != (head); pos = pos->next, prefetch(pos->next))

/* list_for_each_prev - Iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop counter
 * @head:	the head for your list
 */
#define list_for_each_prev(pos, head) \
	for(pos = (head)->prev, prefetch(pos->prev); pos != (head); pos = pos->prev, prefetch(pos->prev))
		
/* list_for_each_safe	-	iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop counter.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head)	\
	for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#ifdef __cplusplus
extern "C"
{
#endif

extern inline void list_add(struct list_head *entry, struct list_head *head);
extern inline void list_add_tail(struct list_head *entry, struct list_head *head);
extern inline void list_del(struct list_head *entry);
extern inline void list_del_init(struct list_head *entry);
extern inline int  list_empty(struct list_head *head);
extern inline void list_splice(struct list_head *list, struct list_head *head);
extern inline void list_replace(struct list_head *old, struct list_head *new_list);
extern inline void list_replace_init(struct list_head *old, struct list_head *new_list);

#ifdef __cplusplus
}
#endif

#endif /* __LINK_LIST_H__ */

