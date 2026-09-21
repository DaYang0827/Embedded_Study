# Two Sum

You are given an array of integers `nums` and an integer `target`, return _indices of the two numbers such that they add up to `target`_.You may assume that each input would have **_exactly_ one solution**, and you may not use the _same_ element twice.

You can return the answer in any order.

**Example 1:**

**Input:** nums = [2,7,11,15], target = 9
**Output:** [0,1]
**Explanation:** Because nums[0] + nums[1] == 9, we return [0, 1].

**Example 2:**

**Input:** nums = [3,2,4], target = 6
**Output:** [1,2]

**Example 3:**

**Input:** nums = [3,3], target = 6
**Output:** [0,1]

解题：
```c
int* twoSum(int* nums, int numsSize, int target, int* returnSize)
{

    int *result = malloc(sizeof(int) * 2);

    for (int i = 0; i < numsSize; i++)
    {
       for (int j = i + 1; j < numsSize; j++)
       {
          if (nums[i] + nums[j] == target)
          {
            result[0] = i;
            result[1] = j;

            *returnSize = 2;

            return(result);
           }
        }
    }

    *returnSize = 0;

    free(result);

    return NULL;  
}
```

# 167. Two Sum II - Input Array Is Sorted

Given a **1-indexed** array of integers `numbers` that is already **_sorted in non-decreasing order_**, find two numbers such that they add up to a specific `target` number. Let these two numbers be `numbers[index1]` and `numbers[index2]` where `1 <= index1 < index2 <= numbers.length`.

Return _the indices of the two numbers_ `index1` _and_ `index2`,  **each incremented by one,**  as an integer array `[index1, index2]` of length 2.

The tests are generated such that there is **exactly one solution**. You **may not** use the same element twice. Your solution must use only constant extra space.

**Example 1:**

**Input:** numbers = [2,7,11,15], target = 9
**Output:** [1,2]
**Explanation:** The sum of 2 and 7 is 9. Therefore, index1 = 1, index2 = 2. We return [1, 2].

**Example 2:**

**Input:** numbers = [2,3,4], target = 6
**Output:** [1,3]
**Explanation:** The sum of 2 and 4 is 6. Therefore index1 = 1, index2 = 3. We return [1, 3].

**Example 3:**

**Input:** numbers = [-1,0], target = -1
**Output:** [1,2]
**Explanation:** The sum of -1 and 0 is -1. Therefore index1 = 1, index2 = 2. We return [1, 2].

```c
int* twoSum(int* numbers, int numbersSize, int target, int* returnSize)
{
    int *result = malloc(sizeof(int) * 2);

    int left = 0;
    int right = numbersSize - 1;

    while (left < right) {
        int sum = numbers[left] + numbers[right];

        if (sum == target) {
            result[0] = left + 1;
            result[1] = right + 1;
            *returnSize = 2;
            return result;
        } else if (sum < target) {
            left++;
        } else {
            right--;
        }
    }

    *returnSize = 0;
    free(result);
    return NULL;
}
```

数组的大小已经排好，使用双指针

```text
left 指向数组开头
right 指向数组结尾

如果 numbers[left] + numbers[right] == target
    返回 left + 1, right + 1

如果和太小
    left++

如果和太大
    right--
```

因为数组有序：

- 和太小，说明左边这个数太小，要往右移动。
- 和太大，说明右边这个数太大，要往左移动。

