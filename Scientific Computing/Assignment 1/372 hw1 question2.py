
#Compensated
import numpy as np
import time
start_time = time.time()

n = 10**6
nums = [1 + (10**6 + 1 - i) * 10**(-8) for i in range(1, n + 1)]


def compensated_sum(nums, precision):
    sum_value = np.float32(0) if precision == 'single' else np.float64(0)
    compensation = np.float32(0) if precision == 'single' else np.float64(0)

    for num in nums:
        y = np.float32(num) if precision == 'single' else np.float64(num)
        y -= compensation
        t = sum_value + y
        compensation = (t - sum_value) - y
        sum_value = t

    return sum_value


compensated_single_precision = compensated_sum(nums, 'single')
compensated_double_precision = compensated_sum(nums, 'double')

print("--- %s seconds ---" % (time.time() - start_time))
print("Compensated Sum (Single Precision):", compensated_single_precision)
print("Compensated Sum (Double Precision):", compensated_double_precision) 















#Pairwise
import numpy as np
import time
start_time = time.time()

n = 10**6
nums = [1 + (10**6 + 1 - i) * 10**(-8) for i in range(1, n + 1)]

threshold=2048
def pairwise_sum(nums, precision):
    if len(nums) <= threshold: 
        if precision == 'single':
            result = np.float32(0)
        else:
            result = np.float64(0)
        for num in nums:
            result += num
    else:
        m = len(nums) // 2
        result = pairwise_sum(nums[:m], precision) + pairwise_sum(nums[m:], precision)
    return result


pairwise_single_precision = pairwise_sum(nums, 'single')
pairwise_double_precision = pairwise_sum(nums, 'double')

print("--- %s seconds ---" % (time.time() - start_time))
print("Pairwise Sum (Single Precision):", pairwise_single_precision)
print("Pairwise Sum (Double Precision):", pairwise_double_precision) 












#Naive
import numpy as np
import time
start_time = time.time()


n = 10**6
nums = [1 + (10**6 + 1 - i) * 10**(-8) for i in range(1, n + 1)]

def naive_sum_single_precision(nums):
    result = np.float32(0)
    for num in nums:
        result += np.float32(num)
    return result

def naive_sum_double_precision(nums):
    result = np.float64(0)
    for num in nums:
        result += np.float64(num)
    return result

result_single_precision = naive_sum_single_precision(nums)
result_double_precision = naive_sum_double_precision(nums)


print("--- %s seconds ---" % (time.time() - start_time))
print("Naive Sum (Single Precision):", result_single_precision)
print("Naive Sum (Double Precision):", result_double_precision)

