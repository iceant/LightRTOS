#ifndef INCLUDED_OS_LIST_H
#define INCLUDED_OS_LIST_H

////////////////////////////////////////////////////////////////////////////////
////
typedef struct os_list_node_s{
    struct os_list_node_s * prev;
    struct os_list_node_s * next;
}os_list_node_t;

typedef os_list_node_t os_list_t;

////////////////////////////////////////////////////////////////////////////////
////
#define OS_LIST_NEXT(N) (N)->next
#define OS_LIST_PREV(N) (N)->prev
#define OS_LIST_PREV_NEXT(N) OS_LIST_NEXT(OS_LIST_PREV(N))
#define OS_LIST_NEXT_PREV(N) OS_LIST_PREV(OS_LIST_NEXT(N))

#define OS_LIST_INIT(N) (OS_LIST_NEXT(N) = OS_LIST_PREV(N) = (N))

#define OS_LIST_IS_EMPTY(L) (OS_LIST_NEXT(L)==(L))

/* X <-- L --> N --> Y */
#define OS_LIST_INSERT_AFTER(L, N) \
do{                                \
    OS_LIST_NEXT_PREV(L) = (N);    \
    OS_LIST_NEXT(N) = OS_LIST_NEXT(L); \
    OS_LIST_NEXT(L) = (N);         \
    OS_LIST_PREV(N) = (L);         \
}while(0)

/* X <-- N <-- L  --> Y */
#define OS_LIST_INSERT_BEFORE(L, N) \
do{                                \
    OS_LIST_PREV_NEXT(L) = (N);    \
    OS_LIST_PREV(N) = OS_LIST_PREV(L); \
    OS_LIST_PREV(L) = (N);         \
    OS_LIST_NEXT(N) = (L);         \
}while(0)


/* X <-- N <-- L  --> Y */
#define OS_LIST_REMOVE(N) \
do{                       \
    OS_LIST_NEXT_PREV(N) = OS_LIST_PREV(N); \
    OS_LIST_PREV_NEXT(N) = OS_LIST_NEXT(N); \
    OS_LIST_INIT(N);      \
}while(0)

#endif /*INCLUDED_OS_LIST_H*/
