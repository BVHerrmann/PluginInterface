/*****************************************************************************/
/*    Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.            */
/*****************************************************************************/
/*    This program is protected by German copyright law and international            */
/*    treaties. The use of this software including but not limited to its            */
/*    Source Code is subject to restrictions as agreed in the license                */
/*    agreement between you and Siemens.                                             */
/*    Copying or distribution is not allowed unless expressly permitted              */
/*    according to your license agreement with Siemens.                              */
/*****************************************************************************/
/*                                                                                   */
/*    P r o j e c t                 &P: PROFINET IO Runtime Software          :P&    */
/*                                                                                   */
/*    P a c k a g e                 &W: PROFINET IO Runtime Software          :W&    */
/*                                                                                   */
/*    C o m p o n e n t             &C: HIF (Host Interface)                  :C&    */
/*                                                                                   */
/*    F i l e                       &F: hif_list.c                            :F&    */
/*                                                                                   */
/*    V e r s i o n                 &V: BC_PNRUN_P07.01.00.00_00.02.00.15     :V&    */
/*                                                                                   */
/*    D a t e    (YYYY-MM-DD) &D: 2019-08-05                                  :D&    */
/*                                                                                   */
/*****************************************************************************/
/*                                                                                   */
/*    D e s c r i p t i o n :                                                        */
/*                                                                                   */
/*    Implements a double linked list                                                */
/*                                                                                   */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID    36
#define HIF_MODULE_ID        LTRC_ACT_MODUL_ID

#include "hif_int.h"
#include "hif_list.h"

#if ( HIF_CFG_USE_HIF == 1 )

/**********************************************************************
 * Functions
 *********************************************************************/


#if (HIF_LIST_CHECK==HIF_LIST_CHECK_FULL)
/**
 * Tests the integrity of the given list.
 * 
 * \note For the case of an endless loop in the list, the number of elements counted is limited by #HIF_LIST_MAX_VERIFY_ELEMENTS.
 *        If you have very large lists, you may need to increase this limitation.
 *
 * @param [in] entry Entry to verfiy
 */ 
LSA_VOID hif_list_verify(HIF_LIST_PTR_TYPE entry)
{
    LSA_INT element_counter;
    HIF_LIST_PTR_TYPE current;
    
    /// check head
    if (entry->next->prev != entry)
    {
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_list_verify() -  entry->next->prev (0x%08x) != entry (0x%08x)", entry->next->prev, entry);
        HIF_FATAL(0);
    }
    
    /// check other elements, if any; don't count more than HIF_LIST_MAX_VERIFY_ELEMENTS elements 
    current=entry->next;
    element_counter=0;
    while (current!=entry && (element_counter <= HIF_LIST_MAX_VERIFY_ELEMENTS))
    {
        element_counter++;
        if ((current->next->prev != current))
        {
            HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_list_verify() -  current->next->prev (0x%08x) != current (0x%08x)", current->next->prev, current);
            HIF_FATAL(0);
        }
        current=current->next;
    }

    current=entry->prev;
    element_counter=0;
    while (current!=entry && (element_counter <= HIF_LIST_MAX_VERIFY_ELEMENTS))
    {
        element_counter++;
        if ((current->prev->next != current))
        {
            HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_list_verify() -  current->next->prev (0x%08x) != current (0x%08x)", current->next->prev, current);
            HIF_FATAL(0);
        }
        current=current->prev;
    }
    
    return;
}
#endif

/**
 * \brief Deletes the element that is located between the two elements \c prev and \c next.
 *
 * @note This is used for internal list manipulation only, since we have to know the
 *             previous and the next element!
 *
 *
 * @param [in] prev The predecessor of the list element to be deleted.
 * @param [in] next The successor of the list element to be deleted.
 */
LSA_VOID hif_list_del_int(HIF_LIST_PTR_TYPE prev, HIF_LIST_PTR_TYPE next)
{
    prev->next=next;
    next->prev=prev;
}


/**
 * Inserts a new list element between the elements prev and next.
 *
 * @note This is used for internal list manipulation only, since we have to know
 *            the previous and the next element!
 *
 *
 * @param [in] new_element The element to insert into the list
 * @param [in] pred The element <b>after</b> which the new element should be inserted.
 *            After successful completion, this element will be the predecessor of the
 *            newly inserted element.
 * @param [in] succ The element <b>before</b> which the new element should be inserted.
 *            After successful completion, this element will be the successor of the
 *            newly inserted element.
 */
LSA_VOID hif_list_add_int(HIF_LIST_PTR_TYPE new_element, HIF_LIST_PTR_TYPE pred, HIF_LIST_PTR_TYPE succ)
{
#if (HIF_LIST_CHECK==HIF_LIST_CHECK_FULL)
    if (new_element==LSA_NULL)
    {
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_list_add_int() -  new_element == LSA_NULL");
        HIF_FATAL(0);
    }
    
    /// prev and next-pointers must be null or point to the element itself 
    if ((new_element->prev != LSA_NULL || new_element->next != LSA_NULL)
     && (new_element != new_element->prev || new_element != new_element->next))
    {
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_list_add_int() -  element is already element in this or another list");
        HIF_FATAL(0);
    }
    hif_list_verify(pred);
#endif
    
    pred->next=new_element;
    succ->prev=new_element;
    new_element->next=succ;
    new_element->prev=pred;

#if (HIF_LIST_CHECK==HIF_LIST_CHECK_FULL)
    hif_list_verify(pred);
#endif
    
#if (HIF_LIST_CHECK==HIF_LIST_CHECK_FAST)
    {
        HIF_LIST_VPTR_TYPE *pred_next_ptr=(HIF_LIST_VPTR_TYPE *)&pred->next;
        HIF_LIST_VPTR_TYPE *succ_prev_ptr=(HIF_LIST_VPTR_TYPE *)&succ->prev;
        HIF_LIST_VPTR_TYPE *new_next_ptr=(HIF_LIST_VPTR_TYPE *)&new_element->next;
        HIF_LIST_VPTR_TYPE *new_prev_ptr=(HIF_LIST_VPTR_TYPE *)&new_element->prev;
        
        if (*pred_next_ptr!=new_element 
         || *succ_prev_ptr!=new_element 
         || *new_prev_ptr!=pred 
         || *new_next_ptr!=succ)
        {
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_list_add_int() -  element is already element in this or another list");
            HIF_FATAL(0);
        }
    }
#endif
    
}

/**
 * Adds a new list element before the given \c head element.
 * If \c head is the "anchor" of the list then the new
 * element is added to the end (tail) of the list.
 *
 *
 * @param [in] new_element The element to be added before the given \c head element
 * @param [in] head The element before which to insert the new element.
 *            If \c head is the "anchor" of the list, then the new element
 *            is added to the end of the list
 */
LSA_VOID hif_list_add_tail(HIF_LIST_PTR_TYPE new_element, HIF_LIST_PTR_TYPE head)
{
    hif_list_add_int(new_element, head->prev, head);
}

/**
 * Adds a new list element after the given \c head element.
 * If \c head is the "anchor" of the list then the new element is added
 * to the beginning of the list.
 *
 *
 * @param [in] new_element Pointer to the element to be added after the given \c head element
 * @param [in] head Pointer to the element after which to insert the new element.
 *            If \c head is the "anchor" of the list, then the new element
 *            is added to the beginning of the list.
 */
LSA_VOID hif_list_add(HIF_LIST_PTR_TYPE new_element, HIF_LIST_PTR_TYPE head)
{
    hif_list_add_int(new_element, head, head->next);
}

/**
 * Deletes the element \c entry from the list.
 *
 * @note The prev and next pointer of the deleted element
 *            remain unchanged and still point the entries within the list!
 *
 *
 * @param [in] entry The element to be deleted from the list.
 */
LSA_VOID hif_list_del(HIF_LIST_PTR_TYPE entry)
{
#if (HIF_LIST_CHECK==HIF_LIST_CHECK_FULL)
    if (entry->prev->next!=entry
            || entry->next->prev!=entry)
    {
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_list_del() -  list corrupted or element not part of a list (anymore)!");
        HIF_FATAL(0);
    }
    hif_list_del_int(entry->prev, entry->next);
    entry->prev = LSA_NULL;
    entry->next = LSA_NULL;
#elif (HIF_LIST_CHECK==HIF_LIST_CHECK_FAST)
    HIF_LIST_PTR_TYPE  prev=entry->prev;
    HIF_LIST_PTR_TYPE  next=entry->next;
        
    HIF_LIST_VPTR_TYPE *pred_next_ptr=(HIF_LIST_VPTR_TYPE*)&entry->prev->next;
    HIF_LIST_VPTR_TYPE *succ_prev_ptr=(HIF_LIST_VPTR_TYPE*)&entry->next->prev;
        
    hif_list_del_int(prev, next);

    if (*pred_next_ptr!=next || *succ_prev_ptr!=prev)
    {
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_list_del() -  list corrupted!");
        HIF_FATAL(0);
    }
#else
    hif_list_del_int(entry->prev, entry->next);
#endif
    //lint --e(818) Pointer parameter 'entry' could be declared ptr to const. Depends no preprocessor defines.
}

/**
 * Deletes the element \c entry from the list and initializes the removed
 * list entry, so that its next and previous pointer point to itself.
 *
 *
 * @param [in] entry The element to be deleted from the list.
 */
LSA_VOID hif_list_del_init(HIF_LIST_PTR_TYPE entry)
{
#if (HIF_LIST_CHECK==HIF_LIST_CHECK_FULL)
    if (entry->prev->next!=entry
     || entry->next->prev!=entry)
    {
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_list_del_init() -  list corrupted or element not part of a list (anymore)!");
        HIF_FATAL(0);
    }
#endif
    hif_list_del_int(entry->prev, entry->next);
    entry->next=entry;
    entry->prev=entry;
}

/**
 * Returns the information whether the list is empty.
 *
 *
 * @param [in] head Head of the list that should be tested
 * @return 1 if the list is empty, 0 otherwise
 */
LSA_INT hif_list_empty(HIF_LIST_CONST_PTR_TYPE head)
{
    return head->next==head;
}

/**
 * Returns the first element from the given double linked list and removes
 * the element from the list
 *
 * @param [in] head Head of the double link list
 * @return First element from the given list or LSA_NULL if the list does not contain
 *        any element
 */
HIF_LIST_PTR_TYPE hif_list_get_first(HIF_LIST_CONST_PTR_TYPE head)
{
    HIF_LIST_PTR_TYPE element;

    if (hif_list_empty(head))
    {
        return LSA_NULL;
    }
    
    element=head->next;

    hif_list_del(element);
    
    return element;
}


/**
 * Returns the first element from the given double linked list, but does
 * not remove the element from the list
 *
 * @param [in] head Head of the double link list
 * @return First element from the given list or LSA_NULL if the list does not contain
 *        any element
 */
HIF_LIST_PTR_TYPE hif_list_read_first(HIF_LIST_CONST_PTR_TYPE head)
{
#if (HIF_LIST_CHECK==HIF_LIST_CHECK_FULL)
    hif_list_verify(head);
#endif
    
    if (hif_list_empty(head))
    {
        return LSA_NULL;
    }
    
    return head->next;
}


/**
 * Utility function to count the number of elements of a list.
 * Iterates over the whole list in order to count its elements.
 *
 * \attention This function has to iterate over the whole list in order to count its elements, so it's takes O(n).
 *
 * @param [in] head Head of the list whose elements should be counted
 *
 * @return The number of elements in the list
 */
LSA_UINT32 hif_list_count(HIF_LIST_CONST_PTR_TYPE head)
{
    LSA_UINT32 count=0;
    HIF_LIST_PTR_TYPE element=head->next;
    
#if (HIF_LIST_CHECK==HIF_LIST_CHECK_FULL)
    hif_list_verify(head);
#endif
    
    while (element != head)
    {
        element = element->next;
        count++;
    }
    return count;

}

#endif // ( HIF_CFG_USE_HIF == 1 )
