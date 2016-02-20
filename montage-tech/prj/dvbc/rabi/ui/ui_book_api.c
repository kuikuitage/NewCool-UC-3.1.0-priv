/**********************************************************************/

/**********************************************************************/
#include "ui_common.h"
#include "mt_time.h"

#define BOOK_INVALID_NODE_INDEX   (0xFF)

static BOOL book_flag = TRUE;

RET_CODE ui_book_init(void)
{
  book_delete_overdue_node();
  ui_set_book_flag(TRUE);
  
  return SUCCESS;
}

void ui_set_book_flag(BOOL is_enable)
{
  book_flag = is_enable;
}

BOOL ui_get_book_flag(void)
{
  return book_flag;
}

#ifdef MARK_BOOK_TIME_ALGORITHM
static BOOL is_time_coverd(utc_time_t *p_s1, utc_time_t* p_e1, utc_time_t *p_s2, utc_time_t* p_e2)
{
  if((time_cmp(p_s1, p_s2, FALSE) == 0) 
    || (time_cmp(p_e1, p_e2, FALSE) == 0))
  {
    return TRUE;
  }

  if((time_cmp(p_s1, p_s2, FALSE) > 0) 
    && (time_cmp(p_s1, p_e2, FALSE) < 0))
  {
    return TRUE;
  }

  //check end time
  if((time_cmp(p_e1, p_s2, FALSE) > 0) 
    && (time_cmp(p_e1, p_e2, FALSE) < 0))
  {
    return TRUE;
  }

  if((time_cmp(p_s2, p_s1, FALSE) > 0) 
    && (time_cmp(p_s2, p_e1, FALSE) < 0))
  {
    return TRUE;
  }

  //check end time
  if((time_cmp(p_e2, p_s1, FALSE) > 0) 
    && (time_cmp(p_e2, p_e1, FALSE) < 0))
  {
    return TRUE;
  }
  
  return FALSE;
}
#else
static BOOL is_time_coverd(utc_time_t *p_s1, utc_time_t* p_e1, utc_time_t *p_s2, utc_time_t* p_e2)
{
	BOOL ret = FALSE;
	s8 orp = time_cmp(p_s1, p_s2, FALSE);
	if(orp < 0)        // p_s1 < p_s2
	{
		// check:    p_e1 <= p_s2    return false
		if( time_cmp(p_e1, p_s2, FALSE) <= 0 )
		{
			ret = FALSE;
		}
		else
		{
			ret = TRUE;
		}
	}
	else if(orp == 0)  // p_s1 == p_s2
	{
		ret = TRUE;
	}
	else               // p_s1 > p_s2
	{
		// check:    p_e2 > p_s1    return ture
		if( time_cmp(p_e2, p_s1, FALSE) > 0 )
		{
			ret = TRUE;
		}
		else
		{
			ret = FALSE;
		}
	}
	
	return ret;
}
#endif

/*!
   to check if the UTC time is booked, if yes, return the booked pg info, else book it.
 */
BOOL is_time_booked(book_pg_t *p_node, u8 except_index)
{
  u8 i = 0;
  book_pg_t tmp_book;
  BOOL ret = FALSE;
  utc_time_t s_time = {0};
  utc_time_t e_time = {0};
  utc_time_t tmp_s = {0};
  utc_time_t tmp_e = {0};
  utc_time_t tmp1_s = {0};
  utc_time_t tmp1_e = {0};
  
  memcpy(&s_time, &(p_node->start_time), sizeof(utc_time_t));
  memcpy(&e_time, &(p_node->start_time), sizeof(utc_time_t));
  time_add(&e_time, &(p_node->drt_time));

  for (i = 0; i < MAX_BOOK_PG; i++)
  {
    if(i != except_index)
    {
      sys_status_get_book_node(i, &tmp_book);

      memcpy(&tmp_s, &(tmp_book.start_time), sizeof(utc_time_t));
      memcpy(&tmp_e, &(tmp_book.start_time), sizeof(utc_time_t));
      time_add(&tmp_e, &(tmp_book.drt_time));

      switch(p_node->book_mode)
      {
        case BOOK_TMR_OFF:
          break;
        case BOOK_TMR_ONCE:
          switch(tmp_book.book_mode)
          {
            case BOOK_TMR_OFF:
              break;
            case BOOK_TMR_ONCE:
                if(is_time_coverd(&s_time, &e_time, &tmp_s, &tmp_e))
                {
                  ret = TRUE;
                }
              break;
            case BOOK_TMR_DAILY:
                if((time_cmp(&s_time, &tmp_s, FALSE) > 0 ) || (( s_time.year == tmp_s.year)
                  && ( s_time.month == tmp_s.month) && ( s_time.day == tmp_s.day)))

                {
                   tmp_s.year = tmp1_s.year = 0;
                   tmp_s.month = tmp1_s.month = 0;
                   tmp_s.day = tmp1_s.day = 0;
                  
                   tmp_e.year = tmp1_e.year = 0;
                   tmp_e.month = tmp1_e.month = 0;
                   tmp_e.day = tmp1_e.day = 0;
                   
                   tmp1_s.hour = s_time.hour;
                   tmp1_s.minute= s_time.minute;
                   tmp1_s.second= s_time.second;
                   
                   tmp1_e.hour = e_time.hour;
                   tmp1_e.minute= e_time.minute;
                   tmp1_e.second= e_time.second; 

                  if(is_time_coverd(&tmp1_s, &tmp1_e, &tmp_s, &tmp_e))
                  {
                    ret = TRUE;
                  }
                }
              break;
            case BOOK_TMR_WEEKLY:
                if(date_to_weekday(&s_time) == date_to_weekday(&tmp_s))
                {
                   tmp_s.year = tmp1_s.year = 0;
                   tmp_s.month = tmp1_s.month = 0;
                   tmp_s.day = tmp1_s.day = 0;
                  
                   tmp_e.year = tmp1_e.year = 0;
                   tmp_e.month = tmp1_e.month = 0;
                   tmp_e.day = tmp1_e.day = 0;

                   tmp1_s.hour = s_time.hour;
                   tmp1_s.minute= s_time.minute;
                   tmp1_s.second= s_time.second;
                   
                   tmp1_e.hour = e_time.hour;
                   tmp1_e.minute= e_time.minute;
                   tmp1_e.second= e_time.second;
                  
                if(is_time_coverd(&tmp1_s, &tmp1_e, &tmp_s, &tmp_e))
                {
                  ret = TRUE;
                }
                }
              break;
            default:
              MT_ASSERT(0);
              break;
          }
          break;
        case BOOK_TMR_DAILY:
          switch(tmp_book.book_mode)
          {
            case BOOK_TMR_OFF:
              break;
            case BOOK_TMR_ONCE:
                if((time_cmp(&s_time, &tmp_s, FALSE) < 0 ) || (( s_time.year == tmp_s.year)
                  && ( s_time.month == tmp_s.month) && ( s_time.day == tmp_s.day)))

                {
                   tmp_s.year = tmp1_s.year = 0;
                   tmp_s.month = tmp1_s.month = 0;
                   tmp_s.day = tmp1_s.day = 0;
                  
                   tmp_e.year = tmp1_e.year = 0;
                   tmp_e.month = tmp1_e.month = 0;
                   tmp_e.day = tmp1_e.day = 0;

                   tmp1_s.hour = s_time.hour;
                   tmp1_s.minute= s_time.minute;
                   tmp1_s.second= s_time.second;
                   
                   tmp1_e.hour = e_time.hour;
                   tmp1_e.minute= e_time.minute;
                   tmp1_e.second= e_time.second;

                  if(is_time_coverd(&tmp1_s, &tmp1_e, &tmp_s, &tmp_e))
                  {
                    ret = TRUE;
                  }
                }
              break;
            case BOOK_TMR_DAILY:
	   case BOOK_TMR_WEEKLY:
                    tmp_s.year = tmp1_s.year = 0;
                   tmp_s.month = tmp1_s.month = 0;
                   tmp_s.day = tmp1_s.day = 0;
                  
                   tmp_e.year = tmp1_e.year = 0;
                   tmp_e.month = tmp1_e.month = 0;
                   tmp_e.day = tmp1_e.day = 0;

                   tmp1_s.hour = s_time.hour;
                   tmp1_s.minute= s_time.minute;
                   tmp1_s.second= s_time.second;
                   
                   tmp1_e.hour = e_time.hour;
                   tmp1_e.minute= e_time.minute;
                   tmp1_e.second= e_time.second;
                  
                if(is_time_coverd(&tmp1_s, &tmp1_e, &tmp_s, &tmp_e))
                {
                  ret = TRUE;
                }
              break;
            default:
              MT_ASSERT(0);
              break;
          }
          break;
        case BOOK_TMR_WEEKLY:
          switch(tmp_book.book_mode)
          {
            case BOOK_TMR_OFF:
              break;
            case BOOK_TMR_ONCE:
	    case BOOK_TMR_WEEKLY:
                if(date_to_weekday(&s_time) == date_to_weekday(&tmp_s))
                {
                     tmp_s.year = tmp1_s.year = 0;
                   tmp_s.month = tmp1_s.month = 0;
                   tmp_s.day = tmp1_s.day = 0;
                  
                   tmp_e.year = tmp1_e.year = 0;
                   tmp_e.month = tmp1_e.month = 0;
                   tmp_e.day = tmp1_e.day = 0;

                   tmp1_s.hour = s_time.hour;
                   tmp1_s.minute= s_time.minute;
                   tmp1_s.second= s_time.second;
                   
                   tmp1_e.hour = e_time.hour;
                   tmp1_e.minute= e_time.minute;
                   tmp1_e.second= e_time.second;
                  
                if(is_time_coverd(&tmp1_s, &tmp1_e, &tmp_s, &tmp_e))
                {
                  ret = TRUE;
                }
               }
              break;
            case BOOK_TMR_DAILY:
             tmp_s.year = tmp1_s.year = 0;
                   tmp_s.month = tmp1_s.month = 0;
                   tmp_s.day = tmp1_s.day = 0;
                  
                   tmp_e.year = tmp1_e.year = 0;
                   tmp_e.month = tmp1_e.month = 0;
                   tmp_e.day = tmp1_e.day = 0;

                   tmp1_s.hour = s_time.hour;
                   tmp1_s.minute= s_time.minute;
                   tmp1_s.second= s_time.second;
                   
                   tmp1_e.hour = e_time.hour;
                   tmp1_e.minute= e_time.minute;
                   tmp1_e.second= e_time.second;
                  
                if(is_time_coverd(&tmp1_s, &tmp1_e, &tmp_s, &tmp_e))
                {
                  ret = TRUE;
                }
              break;
            default:
              MT_ASSERT(0);
              break;
          }
          break;
        default:
          MT_ASSERT(0);
          break;
      }    
    }
  }

  OS_PRINTF("is time booked [%d]\n", ret);
  
  return ret;

}


BOOL is_node_playing(book_pg_t *p_node)
{
  book_pg_t node;
  utc_time_t time;
  BOOL ret = TRUE;
  time_set_t p_set={{0}};
  
  sys_status_get_time(&p_set);
 // if(p_set.gmt_usage == FALSE)
//  {
    time_get(&time, FALSE);//sync flounder which fixed bug 25818 
 // }
  //else
 // {
 //   time_get(&time,TRUE);
 // }

  //node.start_time.value = p_node->start_time.value;
  memcpy(&(node.start_time), &(p_node->start_time), sizeof(utc_time_t));
  
  if (time_cmp(&(node.start_time), &time, TRUE) == 1) //start time is later than current time.
  {
    ret = FALSE;
  }

  printf_time(&(node.start_time), "start time");
  printf_time(&time, "current time");
  OS_PRINTF("is node playing [%d]\n", ret);

  return ret;  
}


void book_get_book_node(u8 index, book_pg_t *p_node)
{
  sys_status_get_book_node(index, p_node);
}


BOOL book_add_node(book_pg_t *p_node)
{
  book_pg_t tmp_node;
  u8 i;

  OS_PRINTF("Add Book Node!\n");
  MT_ASSERT((p_node->pgid != INVALIDID));

  //find a blank node, and add it.
  for (i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &tmp_node);
    if ((tmp_node.pgid == 0)
        && (tmp_node.book_mode == BOOK_TMR_OFF))
    {
      sys_status_add_book_node(i, p_node);
      return TRUE;
    }
  }

  return FALSE;
}

void book_edit_node(u8 index, book_pg_t *p_node)
{
  sys_status_delete_book_node(index);
  sys_status_add_book_node(index, p_node);
}


BOOL book_delete_node(u8 index)
{
  OS_PRINTF("Delete Book Node [%d]\n", index);

  sys_status_delete_book_node(index);

  return TRUE;
}

BOOL book_delete_all_node()
{
  u8 index = 0;
  u8 book_num = 0;
  OS_PRINTF("Delete all Book Node\n");

  book_num = book_get_book_num();
  for(;index < book_num;index ++)
  {
    sys_status_delete_book_node(index);
  }

  return TRUE;
}

u8 book_get_book_num(void)
{
  book_pg_t node;
  u8 i, total = 0;

  for (i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &node);

    if ((node.pgid != 0) && (node.book_mode != BOOK_TMR_OFF))
    {
      total++;
    }
  }

  OS_PRINTF("book_get_book_num :book number is [%d] \n", total);
  return total;
}

u8 book_get_latest_index(void)
{
  u8 i = 0;
  book_pg_t tmp_node;
  utc_time_t tmp_time = {0};
  u8 index = MAX_BOOK_PG;

  memset(&tmp_time, 0xFF, sizeof(utc_time_t));

  for(i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &tmp_node);

    if((tmp_node.pgid != 0)
        && (tmp_node.book_mode != BOOK_TMR_OFF))
    {
      //start_time <= tmp_time
      if(time_cmp(&(tmp_node.start_time), &tmp_time, FALSE) <= 0)
      {
        memcpy(&tmp_time, &(tmp_node.start_time), sizeof(utc_time_t));
        index = i;
      }
    }
  }

  return index;
}



//to check if the latest book node is expired.
u8 book_check_latest_node(u8 index, BOOL drag_node)
{
  book_pg_t tmp_node;
  utc_time_t loc_time;
  utc_time_t end_time;
  utc_time_t dec_time = {0};
  u8 ret = LATEST_NODE_WAIT;

  time_get(&loc_time, FALSE);

  if (index != MAX_BOOK_PG)
  {
    book_get_book_node(index, &tmp_node);
    dec_time.second = 5;
    time_add(&tmp_node.start_time, &dec_time);    

    memcpy(&end_time, &(tmp_node.start_time), sizeof(utc_time_t));
    time_add(&end_time, &(tmp_node.drt_time));

    mtos_printk("start_time: %d, %d, %d\n", tmp_node.start_time.hour, tmp_node.start_time.minute, tmp_node.start_time.second);
    mtos_printk("end_time: %d, %d, %d\n", end_time.hour, end_time.minute, end_time.second);
    mtos_printk("loc_time: %d, %d, %d\n", loc_time.hour, loc_time.minute, loc_time.second);
  
   if(time_cmp(&(tmp_node.start_time), &loc_time, FALSE) > 0)
   {
	if(time_dec(&tmp_node.start_time,&loc_time) == RECORD_TIME_OUT_MS / 1000 \
	&& tmp_node.record_enable)
	{
	return LATEST_NODE_START;
	}
    }
    else if((time_cmp(&(tmp_node.start_time), &loc_time, FALSE) == 0) && (tmp_node.record_enable == 0))
    {
      OS_PRINTF("LATEST_NODE_START\n");
      return LATEST_NODE_START;
    }
    else if(time_cmp(&end_time, &loc_time, FALSE) == 0)
    {
      OS_PRINTF("LATEST_NODE_END\n");

      return LATEST_NODE_END;
    }
  }

  return ret;

}

//when delete pgs, reset latest index.
void book_check_node_on_delete(void)
{
  u8 i;
  book_pg_t book_node;
  dvbs_prog_node_t pg;

  for(i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &book_node);

    if((DB_DVBS_FAILED == db_dvbs_get_pg_by_id(book_node.pgid, &pg)) || (pg.hide_flag == 1))
    {
      //book pg had been deleted, so delete book node.
      book_delete_node(i);
    }
  }
}

void book_overdue_node_handler(u8 index)
{
  book_pg_t node;
  utc_time_t tmp_time = {0};
  utc_time_t loc_time = {0};
    
  book_get_book_node(index, &node);
  
  //delete node or edit it.
  if (node.book_mode == BOOK_TMR_ONCE)
  {
    OS_PRINTF("~~~~book mode once, delete it\n");
    //single timer, delete node
    book_delete_node(index);
  }
  else if (node.book_mode == BOOK_TMR_DAILY)
  {
    OS_PRINTF("~~~~book mode daily, add on day for it\n");
    
    //add one day for start time.
    tmp_time.day = 1;
    
    time_get(&loc_time, FALSE);
    
    while(1)
    {
      time_add(&(node.start_time), &tmp_time);

      if(time_cmp(&loc_time, &(node.start_time), TRUE) < 0)
      {
        break;
      }          
    }
    
    book_edit_node(index, &node);
  }
  else if (node.book_mode == BOOK_TMR_WEEKLY)
  {
    OS_PRINTF("~~~~book_mode daily, add on day for it\n");
    
    //add one day for start time.
    tmp_time.day = 7;
    
    time_get(&loc_time, FALSE);
    
    while(1)
    {
      time_add(&(node.start_time), &tmp_time);

      if(time_cmp(&loc_time, &(node.start_time), TRUE) < 0)
      {
        break;
      }          
    }
    
    book_edit_node(index, &node);
  }
}

u8 book_get_match_node(book_pg_t *p_node)
{
  book_pg_t tmp_node;
  u8 i;

  OS_PRINTF("Add Book Node!\n");
  MT_ASSERT((p_node->pgid != INVALIDID));

  //find a blank node, and add it.
  for (i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &tmp_node);
    if ((tmp_node.pgid == p_node->pgid)
        && (tmp_node.book_mode == p_node->book_mode)
        && (memcmp(&(tmp_node.start_time), &(p_node->start_time), sizeof(utc_time_t)) == 0)
        && (memcmp(&(tmp_node.drt_time), &(p_node->drt_time), sizeof(utc_time_t)) == 0))
    {
      return i;
    }
  }

  return MAX_BOOK_PG;
}

void book_delete_overdue_node(void)
{
  book_pg_t node;
  utc_time_t loc_time = {0};
  utc_time_t end_time = {0};
  u8 i;

  for (i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &node);

    time_get(&loc_time, FALSE);

    memcpy(&end_time, &(node.start_time), sizeof(utc_time_t));
    time_add(&end_time, &(node.drt_time));

    if(time_cmp(&loc_time, &end_time, TRUE) > 0)
    {
      book_overdue_node_handler(i);
    }
  }
}


