#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/uaccess.h>
#include "process_ancestors.h"
#include <linux/syscalls.h>

SYSCALL_DEFINE3(process_ancestors,struct process_info*,info_array, long,size, long*,num_filled){
    

    //set initial
    long temp_numFilled = 0 ;        //num filled
	int childrencount = 0;     //children count
	int siblingcount = 0;      //silbling count
	

    //set type struct
	struct process_info process;
	struct task_struct* curr_task = current;
	struct list_head  *list_head = NULL; 
	

	//check input cases
	if (size <= 0 ){
		return -EINVAL;
	}
	if (!info_array||!num_filled){
		return -EFAULT;
	}
	
	
	//case condition
	while ((temp_numFilled < size) && ((curr_task->parent) != curr_task)){
		//count children
		list_for_each (list_head, &(curr_task->children)){
			childrencount++;
		}
        //count silbling
		list_for_each(list_head, &(curr_task->sibling)){
			siblingcount++;
		}
        
		//stroe infos
		process.pid = curr_task->pid;
		strncpy(process.name, curr_task->comm, ANCESTOR_NAME_LEN);
		process.uid = (curr_task->cred)->uid.val;
		process.nvcsw = curr_task-> nvcsw;
		process.nivcsw = curr_task->nivcsw;
		process.state = curr_task->state;

		//set silbing and children
		process.num_siblings  = siblingcount;
		process.num_children = childrencount;
		
        
        //check fail case
		if((copy_to_user(&(info_array[temp_numFilled]), &process, sizeof(struct process_info))))
				{
					return -EFAULT;
		} 
		temp_numFilled++;
		curr_task = curr_task->parent;
	}
     //check fail case
	if (copy_to_user(num_filled, &temp_numFilled, sizeof(long))){
		return -EFAULT;
	}

	return 0;
}


