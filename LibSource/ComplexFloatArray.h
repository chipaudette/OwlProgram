#ifndef __ComplexFloatArray_h__
#define __ComplexFloatArray_h__

#include "FloatArray.h"
#include "basicmaths.h"
/**
* A structure defining a floating point complex number as two members of type float.
*/
struct ComplexFloat {
  /**
  The real part of the complex number.
  */
  float re;
  
  /**
  The imaginary part of the complex number.
  */
  float im;
  
  /**
  Get the magnitude of the complex number.
  Computes and returns the magnitude of the complex number.
  @return The magnitude of the complex number.
  */
  float getMagnitude(){
    return sqrtf(re*re+im*im);  
  }
  
  /**
  Get the phase of the complex number.
  Computes and returns the phase of the complex number.
  @return The phase of the complex number.
  */  
  float getPhase(){
    return atan2(im,re);
  }
  
  /**
  Set the phase of the complex number.
  Set the phase of the complex number, keeping the magnitude unaltered.
  @param phase The new phase of the complex number
  */
  void setPhase(float phase){
    float magnitude=getMagnitude();
    setPolar(magnitude, phase);
  }
  
  /**
  Set the magnitude of the complex number.
  Set the magnitude of the complex number, keeping the phase unaltered.
  @param magnitude The new magnitude of the complex number
  */
  void setMagnitude(float magnitude){
    float phase=getPhase();
    setPolar(magnitude, phase);
  }
  
  
  /**
  Set magnitude and phase of the complex number.
  @param magnitude The new magnitude of the complex number
  @param phase The new phase of the complex number
  */
  void setPolar(float magnitude, float phase){
    re=magnitude*cosf(phase);
    im=magnitude*sinf(phase);
  }
};

class ComplexFloatArray {
private:
  ComplexFloat* data;
  int size;
public:
  /**Constructor

    Initializes size to 0.
  */
  ComplexFloatArray() :
    data(NULL), size(0) {}
  
  /**
    Constructor.
      
    @param array A pointer to an array of ComplexFloat
    @param size The length of the rray
  */
  ComplexFloatArray(ComplexFloat* array, int size) :
    data(array), size(size) {}
      
  /** 
    The real part of an element of the array.
      
    @param i The index of the element
    @return The real part of the element
  */
  float re(const int i){
    return data[i].re;
  }
  
  /**
    The imaginary part of an element of the array.
    @param i The index of the element
    @return The imaginary part of the element
  */
  float im(const int i){
    return data[i].im;
  }
  
  /**
    The magnitude of an element of the array.
    @param i The index of the element
    @return The magnitude of the element
  */  
  float mag(const int i);
  
  /**
    The magnitudes of the elements of the array.
    @param[out] destination The array where the magnitude values will be stored.
  */  
  void getMagnitudeValues(FloatArray& destination);
  
  /**
    The magnitude squared of an element of the array.
    @param i The index of the element
    @return The magnitude squared of the element
  */  
  float mag2(const int i);
  
  /**
    The squared magnitudes of the elements of the array.
    @param[out] destination The array where the magnitude squared values will be stored.
  */  
  void getMagnitudeSquaredValues(FloatArray& destination);
  
   /**
    The complex conjugate values of the element of the array.
    @param[out] destination The array where the complex conjugate values will be stored.
  */  
  void getComplexConjugateValues(ComplexFloatArray& destination);
  
   /**
    Complex dot product between arrays.
    @param[in] operand2 The second operand of the dot product
    @param[out] result The array where the result of the dot product is stored 
  */  
  void complexDotProduct(ComplexFloatArray& operand2, ComplexFloat& result);

   /**
    Complex by complex multiplication between arrays.
    @param[in] operand2 The second operand of the multiplication
    @param[out] result The array where the result of the multiplication is stored 
  */  
  void complexByComplexMultiplication(ComplexFloatArray& operand2, ComplexFloatArray& result);
  
   /**
    Complex by real multiplication between arrays.
    @param[in] operand2 The second operand of the multiplication
    @param[out] result The array where the result of the multiplication is stored 
  */ 
  void complexByRealMultiplication(FloatArray& operand2, ComplexFloatArray& result);
  int getSize() const{
    return size;
  }
  
  /**
    The value of the element with the maximum magnitude in the array.
    @return The maximum magnitude value in the array.
  */ 
  float getMaxMagnitudeValue();
  
  /**
    The index of the element with the maximum magnitude in the array.
    @return The index of the element with the maximum magnitude in the array.
  */ 
  int getMaxMagnitudeIndex();
  
  /**
   * A subset of the array.
   * Returns an array that points to subset of the memory used by the original array.
   * @param[in] offset the first element of the subset.
   * @param[in] length the number of elments in the new FloatArray.
   * @return the newly created FloatArray.
   * @remarks no memory is allocated by this method. The memory is still shared with the original array.
   * The memory should not be de-allocated elsewhere (e.g.: by calling ComplexFloatArray::destroy() on the original ComplexFloatArray) 
   * as long as the ComplexFloatArray returned by this method is still in use.
   * @remarks Calling ComplexFloatArray::destroy() on a ComplexFloatArray instance created with this method might cause an exception.
  */
  ComplexFloatArray subArray(int offset, int length);
  
  /** Get the real part of the elements of the array.
   * @param[out] buf The array where the real part will be stored.
   */   
  void getRealValues(FloatArray& buf);
  
  /** Get the imaginary part of the elements of the array.
   * @param[out] buf The array where the imaginary part will be stored.
  */   
  void getImaginaryValues(FloatArray& buf);
  
  /** Array by scalar multiplication.
   * @param factor The value by which all the elements of the array are multiplied.
   */
  void scale(float factor);
  
  /**
   * Allows to index the array using array-style brackets.
   * @param index The index of the element
   * @return the Value of the <code>index</code> element of the array
   * Example usage:
   * @code
   * int size=1000;
   * float content[size]; 
   * ComplexFloatArray complexFloatArray(content, size);
   * for(int n=0; n<size; n+=2){//now the ComplexFloatArray can be indexed as if it was an array
   *   content[n]==complexFloatArray[n/2].re; 
   *   content[n+1]==complexFloatArray[n/2].im;
   * }
   * @endcode
  */
  ComplexFloat& operator [](const int index){
    return data[index];
  }
  
  /**
   * Allows to index the array using array-style brackets.
   * 
   * <code>const</code> version of operator[]
  */
  ComplexFloat& operator [](const int i) const{
    return data[i];
  }

  /**
   * Casting operator to ComplexFloat*
   * @return A ComplexFloat* pointer to the data stored in the ComplexFloatArray
  */  
  operator ComplexFloat*() {
    return data;
  }
  
  /**
   * Casting operator to float*
   * @return A float* pointer to the data stored in the ComplexFloatArray
  */ 
  operator float*() {
    return (float *)data;
  }
  
  /**
   * Get the data stored in the ComplexFloatArray.
   * @return a ComplexFloat* pointer to the data stored in the ComplexFloatArray
  */
  ComplexFloat* getData(){
    return data;
  }

  /**
   * Compares two arrays.
   * Performs an element-wise comparison of the values contained in the arrays.
   * @param other the array to compare against.
   * @return <code>true</code> if the arrays have the same size and the value of each of the elements of the one 
   * match the value of the corresponding element of the other, or <code>false</code> otherwise.
  */
  bool equals(const ComplexFloatArray& other) const{
    if(size!=other.getSize()){
      return false;
    }
    for(int n=0; n<size; n++){
      if(data[n].re!=other[n].re || data[n].im!=other[n].im){
        return false;
      }
    }
    return true;
  }
  
  /**
   * Creates a new ComplexFloatArray.
   * Allocates size*sizeof(float) bytes of memory and returns a ComplexFloatArray that points to it.
   * @param size The size of the new ComplexFloatArray.
   * @return A ComplexFloatArray which **data** point to the newly allocated memory and <code>size</code> is initialized to the proper value.
   * @remarks A ComplexFloatArray created with this method has to be destroyed invoking the ComplexFloatArray::destroy() method.
  */
  static ComplexFloatArray create(int size);

  /**
   * Destroys a ComplexFloatArray created with the create() method.
   * @param array The ComplexFloatArray to be destroyed.
   * @remarks The ComplexFloatArray object passed as an argument should not be used again after invoking this method.
   * @remarks A ComplexFloatArray object that has not been created by the ComplexFloatArray::create() method might cause an exception if passed as an argument to this method.
  */
  static void destroy(ComplexFloatArray);

  /**
   * Copies the content of an array into another array.
   * @param[in] source The source array
  */
  void copyFrom(FloatArray source);

  /**
   * Copies the content of a FloatArray into the ComplexFloatArray.
   * @param[in] source The source array
   * @remarks The size of the FloatArray must be twice the size of the ComplexFloatArray.
  */
  void copyFrom(ComplexFloatArray source);
  
  /**
   * Copies an array of ComplexFloat into the array.
   * @param[in] source A pointer to the beginning of the portion of memory to read from.
   * @param[in] length Number of samples to copy.
  */
  void copyFrom(ComplexFloat* source, int length);
  
  /**
   * Copies the content of the ComplexFloatArray into a FloatArray.
   * @param[out] destination The destination array
   * @remarks The size of the FloatArray must be twice the size of the ComplexFloatArray.
  */
  void copyTo(FloatArray destination);

  /**
   * Copies the content of the array to another array.
   * @param[out] destination The destination array
  */
  void copyTo(ComplexFloatArray destination);
  
  /**
   * Copies an array of ComplexFloat into the array.
   * @param[in] destination A pointer to the beginning of the portion of memory to write to.
   * @param[in] length Number of samples to copy.
  */
  void copyTo(ComplexFloat* destination, int length);


  /**
   * Set all the elements in the array.
   * @param[in] value All the elements are set to this value.
  */
  void setAll(ComplexFloat value);
  
  /**
   * Set all the elements in the array.
   * @param[in] value The real and imaginary parts of all the elements in the array are set to this value.
  */
  void setAll(float value);
  
  /**
   * Set all the elements in the array.
   * @param[in] valueRe The real part of every element of the the array will be set to this value.
   * @param[in] valueIm The imaginary pary of rvery element of the the array will be set to this value.
  */
  void setAll(float valueRe, float valueIm);

  /**
   * Set all the elements in the array using polar coordinates.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[in] phase An array containing the phases.
  */
  void setPolar(FloatArray magnitude, FloatArray phase);
  
  /**
   * Set a range of elements in the array using polar coordinates.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[in] phase An array containing the phases.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
  */  
  void setPolar(FloatArray magnitude, FloatArray phase, int offset, int count);

  /**
   * Set the phase of the elements of the array, leaving the magnitude unchanged.
   * @param[in] phase An array containing the phases.
  */
  void setPhase(FloatArray phase);

  /**
   * Set the phase of a range of elements of the array, leaving the magnitude unchanged.
   * @param[in] phase An array containing the phases.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
  */
  void setPhase(FloatArray phase, int offset, int count);
  
  /**
   * Set the phase of the elements of an array, using the magnitude from the current array.
   * @param[in] phase An array containing the phases.
   * @param[out] destination The destination array.
  */
  void setPhase(FloatArray phase, ComplexFloatArray destination);
  
  /**
   * Set the phase of a range of the elements of an array, using the magnitude from the current array.
   * 
   * The <code>offset</code> and <code>count</code> specified apply to both the source and <code>destination</code> arrays. 
   * Elements of the <code>destination</code> array outside this range will not be affected.
   * @param[in] phase An array containing the phases.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
   * @param[out] destination The destination array.
  */
  void setPhase(FloatArray phase, int offset, int count, ComplexFloatArray destination);


  /**
   * Set the magnitude of the elements of the array, leaving the phase unchanged.
   * @param[in] magnitude An array containing the magnitudes.
  */
  void setMagnitude(FloatArray magnitude);

/**
   * Set the magnitude of a range of elements of the array, leaving the phase unchanged.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
  */
  void setMagnitude(FloatArray magnitude, int offset, int count);
  
  /**
   * Set the magnitude of the elements of an array, using the phase from the current array.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[out] destination The destination array.
  */
  void setMagnitude(FloatArray magnitude, ComplexFloatArray destination);
  
  /**
   * Set the magnitude of a range of the elements of an array, using the phases from the current array.
   * 
   * The <code>offset</code> and <code>count</code> specified apply to both the source and <code>destination</code> arrays. 
   * Elements of the <code>destination</code> array outside this range will not be affected.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
   * @param[out] destination The destination array.
  */
  void setMagnitude(FloatArray magnitude, int offset, int count, ComplexFloatArray destination);
};

#endif // __ComplexFloatArray_h__
