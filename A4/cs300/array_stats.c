#include "array_stats.h"
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
asmlinkage long sys_array_stats(struct array_stats *stats, long data[], long size);
SYSCALL_DEFINE3(array_stats,struct array_stats*,stats, long* ,data, long, size){
	
	long temp_copy= 0;
	int ret = 0;
	int i =  0;
	int flag = 0;

	struct array_stats temp_stats;
	temp_stats.sum=0;

	//check if the input array is empty 
	if (!stats){
		return -EFAULT;
	}
	if (size<=0){
		return -EINVAL;
	}

	for (i=0; i<size; i++){
		
		ret = copy_from_user(&temp_copy, &data[i], sizeof(data[i]));

		//check if copy_from_user failed
		if (ret != 0){
			return -EFAULT;
		}

        //intital val
		if (flag==0){
            temp_stats.min = temp_stats.max = temp_copy;
			flag=1;
		}


		if (flag==1){
			//replace min
			if (temp_stats.min > temp_copy){
				temp_stats.min = temp_copy;
			}

			//replace max
			
			if (temp_stats.max < temp_copy){
				temp_stats.max = temp_copy;

			}
	    }

		//add up all value
		temp_stats.sum += temp_copy;
	}
	ret = copy_to_user(stats, &temp_stats, sizeof(temp_stats));
	
	//final check
	if (ret != 0){
			return -EFAULT;
	}
	return 0;
}
