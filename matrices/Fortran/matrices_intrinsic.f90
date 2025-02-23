Module MatrixRoutines
  ! module defining required allocatable matrices, C=A*B
  ! and routines used
  ! (c) mkbane (2023-2025)
  real, dimension(:,:), allocatable:: A, B, C
  integer, dimension(:), allocatable:: seed
  integer, parameter:: DEFAULT_N = 1024
Contains
  Subroutine InitMatrices(N)
    ! fill A,B with random; fill C with zeroes

    Implicit None
    integer, intent(IN):: N
    integer:: numSeeds, ierr

    ! allocate and initialise
    allocate(A(N,N),STAT=ierr)
    if (ierr /= 0) stop "cannot allocate A"
    allocate(B(N,N),STAT=ierr)
    if (ierr /= 0) stop "cannot allocate B"
    allocate(C(N,N),STAT=ierr)
    if (ierr /= 0) stop "cannot allocate C"

!    call random_seed(SIZE=numSeeds)
!    write(*,*) 'expected #ints for seeds: ', numSeeds
!    allocate(seed(numSeeds),STAT=ierr)
!    if (ierr /= 0) stop "cannot allocate PRNG seed array"
!    seed = 101
!    call random_seed(PUT=seed(1:numSeeds))
    call fillValues(A, N, 20.0)
    call fillValues(B, N, 05.0)
    call fillZeroes(C, N)
    
  End Subroutine InitMatrices

  Subroutine fillRandom(X, N)
    ! fill given 2D array with random numbers
    real, dimension(N,N):: X
    integer, intent(IN):: N
    integer i,j
    do i=1,N
       call random_number(X(i,:))
    end do
  End Subroutine FILLRANDOM

  Subroutine fillValues(X, N, seed)
    ! function to fill array with known numbers  between -seed and almost +seed
    ! to create same matrix as C example
    ! "almost" is to avoid symmetry for square matrices
    real, dimension(N,N):: X
    real, intent(IN):: seed
    integer, intent(IN):: N
    real:: step
    integer i,j
    integer k
    step= 0.95*(2.0*seed / (N * N - 1));
    k=0
    do i=1,N
       do j=1,N
          X(i,j) = -seed + k*step
          k = k + 1
       end do
    end do
  End Subroutine fillValues

  Subroutine fillZeroes(X, N)
    ! fill given 2D array with zero
    real, dimension(N,N):: X
    integer, intent(IN):: N
    integer i,j
    do i=1,N
       do j=1,N
          X(i,j) = 0.0
       end do
    end do
  End Subroutine fillZeroes

  Subroutine printMatrix(X, N)
    ! output given 2D array 
    real, dimension(N,N):: X
    integer, intent(IN):: N
    integer i,j
    do i=1,N
       write(*,*) "ROW ",i
       do j=1,N
          write(*,*) X(i,j), ","
       end do
       write(*,*)
    end do
  end Subroutine printMatrix

  Real Function getFrob(X, N)
    real, dimension(N,N):: X
    integer, intent(IN):: N
    integer:: i,j
    real:: sum
    double precision:: val
    sum = 0.0
    ! do i=1, N
    !    do j=1, N
    !       val = X(i,j)
    !       sum = sum + abs(val) * abs(val)
    !    end do
    ! end do

    ! mimic C code ordering
    do j=1, N
       do i=1, N
          val = X(i,j)
          sum = sum + abs(val) * abs(val)
       end do
    end do
    getFrob = sqrt(sum)
  end Function getFrob
  
End Module MatrixRoutines

Program myMM
  ! Fortran code to form C=A*B for a given N
  ! and to output Frobenius norm of C
  ! (c) mkbane (2023-2025)
  USE MatrixRoutines
  Implicit None

!  INTERFACE
!     SUBROUTINE InitMatrices(N)
!     END SUBROUTINE InitMatrices
!  END INTERFACE

  integer:: N
  integer:: i,j,k
  character(len=8):: arg  ! string to represent "N"

  ! use command line arg if avail to determine size of matrices
  if (command_argument_count() >= 1) then
     call get_command_argument(1, arg)
     read(arg,*) N
     write(*,*) "Each array is ", N, " by ", N
     if (command_argument_count() > 1) write(*,*) "(ignoring other parameters)"
  else
     N = DEFAULT_N
     write(*,*) "Each array is ", N, " by ", N
  end if
  
  call InitMatrices(N)

  
  ! matmul using intrinsic
  C = matmul(A,B)


  ! output the Frobenius norm
  write(*,*) "C has Frobenius norm:", getFrob(C,N)

End Program myMM
