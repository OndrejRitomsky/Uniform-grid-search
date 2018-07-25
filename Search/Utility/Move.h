#pragma once


template<typename T>
struct RemoveReference;

template<typename T>
struct RemoveReference { typedef T Type; };

template<typename T>
struct RemoveReference<T&> { typedef T Type; };

template<typename T>
struct RemoveReference<T&&> { typedef T Type; };




template<typename T>
inline typename RemoveReference<T>::Type&& Move(T&& t) {
	return static_cast<typename RemoveReference<T>::Type&&>(t);
}

/*
template<typename T>
inline typename T&& Forward(typename RemoveReference<T>::Type& t) {
	return static_cast<T&&>(t);
}

template<typename T>
inline typename T&& Forward(typename RemoveReference<T>::Type&& t) {
	return static_cast<T&&>(t);
}*/
