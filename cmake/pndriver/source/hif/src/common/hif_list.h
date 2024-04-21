#ifndef _HIF_LIST_H_
#define _HIF_LIST_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_list.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Declarations for a double linked list                                    */
/*                                                                           */
/*****************************************************************************/

#if ( HIF_CFG_USE_HIF == 1 )
/**********************************************************************************************
 * Includes
 ***********************************************************************************************/

/**********************************************************************************************
 * Defines
 ***********************************************************************************************/
  
/**
 * Maximal number of list elements for #hif_list_verify(). 
 */
#define HIF_LIST_MAX_VERIFY_ELEMENTS 10000
	
///< Value for HIF_LIST_CHECK that denotes not to do any checks
#define HIF_LIST_CHECK_NONE 0
///< Value for HIF_LIST_CHECK that denotes not to do fast checks after delete/add that just validates the element was successfully deleted/added
#define HIF_LIST_CHECK_FAST 1
///< Value for HIF_LIST_CHECK that denotes to do full list checks (iterate over the whole list) before and after every manipulation
#define HIF_LIST_CHECK_FULL 2
	
#define HIF_LIST_CHECK HIF_LIST_CHECK_NONE
  
/**********************************************************************************************
 * HIF declaration of double linked list
 ***********************************************************************************************/
 /**
  * List structure for a double linked list.
  * Every structure used within a double linked list must have a member of this type.
  * \n
  * The same type \c hif_list is used both for linking the elements within the list and for the
  * head (anchor) of the list. The \c next element of the last list item points to the head
  * (anchor) of the list. The \c next element of the head (anchor) points to the first item of the
  * list and the \c prev element points to the last element of the list
  *
  * \note In order to determine the end of the list the list head has to be known!
  */
typedef struct hif_list_s
{
	struct hif_list_s *next;                            ///< The next entry in the double linked list
	struct hif_list_s *prev;                            ///< The previous entry in the double linked list
} HIF_LIST_TYPE, *HIF_LIST_PTR_TYPE;

typedef HIF_LIST_PTR_TYPE volatile HIF_LIST_VPTR_TYPE;  ///< volatile pointer to an HIF list element 
typedef HIF_LIST_TYPE const* HIF_LIST_CONST_PTR_TYPE;   ///< const pointer to an HIF list element 
  
  
/**********************************************************************************************
 * HIF implementation of double linked list
 ***********************************************************************************************/

/**
 * Converts the given pointer that points to an \c element within the structure \c type 
 * into a pointer to the base of the structure \c type.
 * 
 * This macro is useful if you concatenated instances of a structure using an #HIF_LIST_TYPE 
 * and the #HIF_LIST_TYPE element is NOT the first element of the structure. You can then use 
 * this macro to calculate the base of the structure out of a pointer to the #HIF_LIST_TYPE 
 * element.  
 *  
 * @param type Type of the structure
 * @param element Element within \c type where the given \c pointer points to
 * @param pointer Pointer to the \c element within \c type
 * 
 * @return element Pointer to the the base of the given structure \c type that contains \c element 
 */
#define HIF_LIST_STRUCT_BASE(type, element, pointer) (type*)(LSA_VOID_PTR_TYPE)(((LSA_UINT32)pointer)-(offsetof(type, element)))
  
/**
 * Determines if the end of a list is reached. If the given \b object is the
 * list head given in \b list, 1 is returned. If object is an ordinary list element 
 * 0 is returned 
 *
 * @param object The list element to test whether it is equal to the list head 
 *    (which means the end of the list is reached)
 * @param catenation Element within the structure of \b object which is used to
 *    concatenate the list elements
 * @param list The list itself
 *   
 * @return 1 if \b object is the end of the list (=list head) 0 if object is
 *    an element of the list itself
 */
#define HIF_LIST_END(object, catenation, list) (&object->catenation==&list)

/**
 * Initializes a newly created double linked list.
 *
 * @param name The name of the double linked list to initialize
 */
#define HIF_LIST_INIT(name) { &(name), &(name) }
/**
 * Defines and initializes a double linked list.
 *
 * @see HIF_LIST_INIT
 * @param name The name of the double linked list to create
 */
#define hif_list(name) \
    (HIF_LIST_TYPE (name)=HIF_LIST_INIT(name))

/**
 * Initializes the head of an already created double linked list.
 * The next- and prev-pointer are set to point to the head itself
 *
 * @param ptr Pointer to the list to be initialized
 */
#define HIF_INIT_LIST(ptr) (ptr)->next=(ptr);(ptr)->prev=(ptr);
  
/**
 * Returns and removes the first element of the given list.
 * The element is casted to the given \b type
 * 
 * @param head The head of the list from which the first element should be returned and deleted
 * @param type The type to which the element should be casted
 */
#define HIF_LIST_GET_FIRST_TYPE(head, type) (type)(LSA_VOID_PTR_TYPE)hif_list_get_first(head)

/**
 * Returns the successor of the given /b element in the list.
 * The successor is casted to the given type
 *
 * @param element Element for which the successor should be returned
 * @param type Type to which the successor should be casted
 */
#define HIF_LIST_NEXT(element, type) ((type)(LSA_VOID_PTR_TYPE)(element)->next)

/**
 * Gets the element from the list pointed to by \c ptr.
 * The element is casted to \c type.
 *
 * @param ptr The pointer to the element
 * @param type The type to be returned
 * @param member Member within the \c type structure that acts as the \c HIF_LIST_TYPE header.
 */
#define HIF_LIST_ENTRY(ptr, type, member) \
    ((type *)((LSA_CHAR *)(ptr)-(LSA_UINT32)(&((type*)0)->member)))

/**
 * Iterates over the list, using pos to indicate the current element. Pos can be of any
 * structure (provided that the structure has got a element of type #HIF_LIST_TYPE at first position).
 * The value assigned to pos is casted into the right type. 
 *
 * @param pos Pointer to a \ref HIF_LIST_TYPE type that holds the current element during
 *      iteration
 * @param head The list to be iterated over.
 * @param type Type of pos. When a new value is assigned to pos, it is casted into this type
 */
#define HIF_LIST_FOR_EACH(pos, head, type) \
  for (pos=(type)(head)->next;pos!=(type)(head);pos=(type)((HIF_LIST_PTR_TYPE)pos)->next)

/*----------------------------------------------------------------------------*/

/**
 * Deletes the element that is located between the two elements \c prev and \c next.
 *
 * @note This is used for internal list manipulation only, since we have to know the
 *       previous and the next element!
 *
 * @date 25.05.2005
 *
 * @param prev The predecessor of the list element to be deleted.
 * @param next The successor of the list element to be deleted.
 */
extern LSA_VOID hif_list_del_int(HIF_LIST_PTR_TYPE prev, HIF_LIST_PTR_TYPE next);

/**
 * Inserts a new list element between the elements prev and next.
 *
 * @note This is used for internal list manipulation only, since we have to know
 *      the previous and the next element!
 *
 * @date 25.05.2005
 *
 * @param new_element The element to insert into the list
 * @param pred The element <b>after</b> which the new element should be inserted.
 *      After successful completion, this element will be the predecessor of the
 *      newly inserted element.
 * @param succ The element <b>before</b> which the new element should be inserted.
 *      After successful completion, this element will be the successor of the
 *      newly inserted element.
 */
extern LSA_VOID hif_list_add_int(HIF_LIST_PTR_TYPE new_element, HIF_LIST_PTR_TYPE pred, HIF_LIST_PTR_TYPE succ);

/**
 * Adds a new list element before the given \c head element.
 * If \c head is the "anchor" of the list then the new
 * element is added to the end (tail) of the list.
 *
 * @date 25.05.2005
 *
 * @param new_element The element to be added before the given \c head element
 * @param head The element before which to insert the new element.
 *      If \c head is the "anchor" of the list, then the new element
 *      is added to the end of the list
 */
extern LSA_VOID hif_list_add_tail(HIF_LIST_PTR_TYPE new_element, HIF_LIST_PTR_TYPE head);

/**
 * Adds a new list element after the given \c head element.
 * If \c head is the "anchor" of the list then the new element is added
 * to the beginning of the list.
 *
 * @date 25.05.2005
 *
 * @param new_element Pointer to the element to be added after the given \c head element
 * @param head Pointer to the element after which to insert the new element.
 *      If \c head is the "anchor" of the list, then the new element
 *      is added to the beginning of the list.
 */
extern LSA_VOID hif_list_add(HIF_LIST_PTR_TYPE new_element, HIF_LIST_PTR_TYPE head);

/**
 * Deletes the element \c entry from the list.
 *
 * @note The prev and next pointer of the deleted element
 *      remain unchanged and still point the entries within the list!
 *
 * @date 25.05.2005
 *
 * @param entry The element to be deleted from the list.
 */
extern LSA_VOID hif_list_del(HIF_LIST_PTR_TYPE entry);

/**
 * Deletes the element \c entry from the list and initializes the removed
 * list entry, so that its next and previous pointer point to itself.
 *
 * @date 25.05.2005
 *
 * @param entry The element to be deleted from the list.
 */
extern LSA_VOID hif_list_del_init(HIF_LIST_PTR_TYPE entry);

/**
 * Returns the information whether the list is empty.
 *
 * @date 20.06.2005
 *
 * @param head Head of the list that should be tested
 * @return 1 if the list is empty, 0 otherwise
 */
extern LSA_INT hif_list_empty(HIF_LIST_CONST_PTR_TYPE head);

/**
 * Returns the first element from the given double linked list and removes
 * the element from the list
 *
 * @param head Head the double link list
 * @return First element from the given list or LSA_NULL if the list does not contain
 *    any element
 */
extern HIF_LIST_PTR_TYPE hif_list_get_first(HIF_LIST_CONST_PTR_TYPE head);

/**
 * Returns the first element from the given double linked list, but does
 * not remove the element from the list
 *
 * @param head Head the double link list
 * @return First element from the given list or LSA_NULL if the list does not contain
 *    any element
 */
extern HIF_LIST_PTR_TYPE hif_list_read_first(HIF_LIST_CONST_PTR_TYPE head);

/**
 * Utility function to count the number of elements of a list.
 * Iterates over the whole list in order to count its elements.
 *
 * \attention This function has to iterate over the whole list in order to count its elements, so it's takes O(n).
 *
 * @param head Head of the list whose elements should be counted
 *
 * @return The number of elements in the list
 */
extern LSA_UINT32 hif_list_count(HIF_LIST_CONST_PTR_TYPE head);

#endif // ( HIF_CFG_USE_HIF == 1 )

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*        */
/*****************************************************************************/
#endif  /* of HIF_LIST_H */
