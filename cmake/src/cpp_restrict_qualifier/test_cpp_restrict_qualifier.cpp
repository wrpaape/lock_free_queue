// attempt to compile with -DCPP_RESTRICT_QUALIFIER=<QUALIFIER>
int
set_and_add(int *CPP_RESTRICT_QUALIFIER ptr1,
            int *CPP_RESTRICT_QUALIFIER ptr2)
{
    *ptr1 = *ptr2;
    return *ptr1 + *ptr2;
}

int
main(int argc,
     char *argv[])
{
    static int zero;

    return set_and_add(&argc,
                       &zero);
}
