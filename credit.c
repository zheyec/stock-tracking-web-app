//Read a credit card number and prints if its valid
#include <stdio.h>
#include <cs50.h>

int main(void)
{
    //Read card number
    long long card_num;
    do
    {
        card_num = get_long_long("Number: ");
    }
    while (card_num == INT_MAX);

    //Store the number as separate digits
    int j = 0;
    int digits[20];
    while (card_num > 0)
    {
        digits[j] = card_num % 10;
        card_num /= 10;
        j++;
    }
    j--;

    //Calculate the sum of the digits multiplied by 2
    int pin = 1;
    int sum = 0;
    int temp;
    while (pin <= j)
    {
        temp = digits[pin] * 2;
        sum += temp % 10;
        sum += temp / 10;
        pin += 2;
    }

    //Add other digits
    pin = 0;
    while (pin <= j)
    {
        sum += digits[pin];
        pin += 2;
    }

    //Check if valid
    if (sum % 10 == 0)
    {
        int first_two = digits[j] * 10 + digits[j - 1];
        int first = digits[j];
        if (j == 14 && (first_two == 34 || first_two == 37))
        {
            printf("AMEX\n");
        }
        else if (j == 15 && first_two >= 51 && first_two <= 55)
        {
            printf("MASTERCARD\n");
        }
        else if ((j == 12 || j == 15) && first == 4)\
        {
            printf("VISA\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}