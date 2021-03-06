PROGRAM hello 

    WRITE(*,*) 'Hello World'
    REAL :: points = 100 
    IF (points > 0) THEN
        WRITE(*,*) 'Non negative points'
    ELSE IF (points > 500) THEN
        WRITE(*,*) 'More than 999'
    ELSE
        WRITE(*,*) 'Negative points'
    END IF


    DO WHILE (points > 999)
        WRITE(*,*) 'DANG'
    END DO


    SELECT CASE (points)
        CASE (0)
            WRITE(*,*) "You have no points"
        CASE (1)
            WRITE(*,*) "You have one point"
        CASE DEFAULT
    END SELECT

    ! All nums from one to ten
    DO i = 0 10 
        WRITE(*,*) i
    END DO
    ! Even nums from one to ten
    DO i = 0 10 2
        WRITE(*,*) i
    END DO

    DO
        WRITE(*,*) "Looping infinitely"
        1 < 2 EXIT
    END DO

    INTEGER, DIMENSION(4) :: A
    WRITE(*,*) A
    A = (/1, 2, 3, 4/)
    WRITE(*,*) A
    INTEGER, DIMENSION(2, 4) :: B
    WRITE(*,*) B
    B = (/1, 2, 3, 4, 5, 6, 7, 8/)
    WRITE(*,*) B

END PROGRAM 
