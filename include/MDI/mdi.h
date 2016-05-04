/*
 * Machine Description Interface C API
 *
 * This software is delivered under the terms of the MIT License
 *
 * Copyright (c) 2016 STMicroelectronics
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 *
 * @file mdi.h
 *
 * @brief Machine Description Interface, a C abstract interface to MDD.
 *
 * The Machine Description Interface C API is an abstract interface
 * that must be implemented by a MDI compatible machine description
 * system.
 *
 * The MDI interface is used to access information on machine
 * description objects as described in the MDS/MDD pseudo-relational
 * databse model.
 *
 * The interface itself is a list of abstract objects accessors
 * to object instances fields.
 *
 * The MDI objects types correspond to the MDD database elements.
 * The MDI objects fields correspond to the MDD database attributes for
 * an element.
 *
 * As the machine description is a pseudo-relational model, actually
 * each object instance as generally an assosiated identifier which
 * can be used as a dense index to identify each object.
 *
 * For instance an object abstract type for an instruction
 * (MDI_Instruction_t) could be simply implemented as the corresponding
 * identifier type (as returned by MDI_Instruction_idx()).
 * In which case the implementation
 * could use the instruction object directly as an index into some
 * tabular representation of instruction fields.
*
 * Note that the MDI abstract interface is designed from the MDD
 * machine description database schema, but an implementation may be
 * derived from any source, not necessarily the MDS system.
 * In particular, if one has a specific machine description API, an
 * implementation of the MDI could be a simple bridge to the actual
 * machine model.
 * Of course this is possible only if the two machine models are
 * somewhat compatible in term of described objects.
 *
 * Implementation notes at revision 0.1.0:
 *   - The current version defines a partial interface to
 *     the full MDD object model, this is to be completed.
 *   - A conformant implementation should implement the
 *     full interface; though a partial implementation is
 *     acceptable for dedicated uses. In this case the
 *     missing implementation should raise a link time
 *     error. Said otherwise, do not provide an empty
 *     implementation, prefer to let the error raise at
 *     compile time.
 *   - For usage in binary utilities tools, such as
 *     assemblers, decoders, simulators, the main objects
 *     of interest is the MDI_Instruction_t which provides
 *     description in particular for parsing, decoding,
 *     execution and operator properties.
 *   - For usage in compiler the MDI_Operator_t provides
 *     additional information that may have been generated
 *     from the MDI_Instruction_t corresponding instruction.
 *     Also the MDI_Opcode_t provides more specific information
 *     regarding enconding constraints in some cases.
 *     Though, both these objects are not fully described.
 *
 */

#ifndef _MDI_H_INCLUDED
#define _MDI_H_INCLUDED

/**
 * @name MDI API Revision Values
 *
 * Client side API expected revision identifiers.
 *
 * Use these Macros from the client or implementation
 * to get the expected interface revision.
 * It is the responsibility of the client to verify
 * whether an actual implementation matches this version
 * information though MDI_interface_revision().
 * @{
 */
/** Full revision for this interface. */
#define MDI_VERSION_REV MDI_VERSION_MAKE_REV(MDI_VERSION_MAJOR,MDI_VERSION_MINOR,MDI_VERSION_PATCH)
#define MDI_VERSION_MAJOR 0  /**< Major version, incompatible both way when differ. */
#define MDI_VERSION_MINOR 1  /**< Minor version, compatible if implementation is greater. */
#define MDI_VERSION_PATCH 0  /**< Patch version, compatible both way when differ. */
/**@}*/

/**
 * @name MDI API Revision Constructors
 * 
 * Construct API Revision.
 *
 * Use these Macros to get from the sortable full revision
 * the major, minor, patch versions and vice-versa.
 * @{
 */
/** Build the full revision ordered identifier. */
#define MDI_VERSION_MAKE_REV(major, minor, patch) (((major)*10000 + (minor) * 100 + (patch)))
#define MDI_VERSION_REV_MAJOR(rev) (((rev)/10000)%100) /**< Get major version from revision. */
#define MDI_VERSION_REV_MINOR(rev) (((rev)/100)%100)   /**< Get minor version from revision. */
#define MDI_VERSION_REV_PATCH(rev) ((rev)%100)         /**< Get patch version from revision. */
/**@}*/

/**
 * @name MDI API Revisions Checks
 *
 * Check client and implementation revisions compatibilities.
 * @{
 */
/**
 * @brief Check revisions compatibility.
 *
 * Check wheter the @p actual revision id compatible with the
 * @p expected revision.
 *
 * Interface maintainers should ensure that, for a given
 * major version, minor version is backward compatible with older
 * minor revision clients and patch version is both backward and
 * forward compatible.
 */
#define MDI_VERSION_REV_COMPATIBLE(actual, expected)                    \
    (MDI_VERSION_REV_MAJOR(actual) == MDI_VERSION_REV_MAJOR(expected) && \
     MDI_VERSION_REV_MINOR(actual) >= MDI_VERSION_REV_MINOR(expected))
/**@}*/


/**
 * @name MDI API Interface Scopes
 *
 * API scopes declarators.
 * @{
 */
/** Default API functions declarator, may be overriden before inclusion. */
#ifndef MDI_INTERFACE
#define MDI_INTERFACE extern
#endif
/**@}*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <inttypes.h>


/**
 * @defgroup MDI_scalars MDI API Scalar Types
 *
 * Abstract Scalar types
 *
 * All scalar types used in the API are defined to be sufficiently
 * large for all supported architectures.
 *
 * The types are defined though typedefs and the associated printf
 * format specifier  are provided.
 */
/**@{*/
typedef uint32_t MDI_rev_t;     /**< API revision/versions holder. */
#define MDI_PRIrev PRIu32       /**< Print format for MDI_rev_t. */
typedef uint32_t MDI_idx_t;     /**< All static objects dense identifiers. */
#define MDI_PRIid PRIu32        /**< Print format for MDI_idx_t. */
typedef int32_t MDI_res_t;      /**< Return status, 0 is success. */
#define MDI_PRIres PRIi32       /**< Print format for MDI_res_t. */
typedef const char *MDI_str_t;  /**< C string (0 terminated) of char, always const. */
typedef char *MDI_str_mut_t;    /**< Mutable C string of char. */
#define MDI_PRIstr "s"          /**< Print format for MDI_str_t and MDI_str_mut_t. */
typedef const char *MDI_ptr_t;  /**< Buffer pointer, always const. */
typedef char *MDI_ptr_mut_t;    /**< Mutable buffer pointer. */
#define MDI_PRIptr "p"          /**< Print format for MDI_ptr_t and MDI_ptr_mut_t. */
typedef intptr_t MDI_size_t;    /**< Buffer size or offset as integer, holds all pointer values. */
#define MDI_PRIsize PRIiPTR     /**< Print format for MDI_size_t. */
/**@}*/

/**
 * @defgroup MDI_objects MDI API Abstract Base Type
 *
 * All non scalar API objects use an opaque type from the client side.
 *
 * The MDI_object_t opaque type can be used to hold any MDD or
 * interface objects, though prefer to use the specialized types
 * when available.
 *
 * Note that while the abstraction here uses a pointer to
 * an anonymous type, it is perfectly legal to have a value
 * of 0 for an instance of this type.
 *
 * The implementation holds the actual information which is
 * not exposed through this abstract API.
 * Only in the implementation one may use static cast conversions
 * to get the concrete type from the abstract type and
 * return the abstract type.from the concrete type.
 */
/**@{*/
typedef const void *MDI_object_t; /**< Opaque object type. 0 or NULL may be valid instances. */
typedef void *MDI_object_mut_t;   /**< Mutable object type. 0 or NULL may be valid instances. */
#define MDI_PRIobj "p"                 /**< Print format for MDI_obj_t. */
/**@}*/

/**
 * @defgroup MDI_interface MDI API Interface Object
 *
 * Interface object for the management of the interface instances.
 *
 */
/**@{*/
/**
 * @brief MDI abstract interface instance type.
 *
 * An instance of this interface is retrieved through
 * the MDI_interface_init() factory method.
 * An implementation providing a single platform
 * definition may return a singleton object or a
 * dummy value for this abstact object.
 *
 * This is an object type which does not
 * correspond to any MDD architectur description type.
 * Hence the lower case name 'interface', otherwie,
 * MDD object type names are CamelCase.
 */
typedef MDI_object_t MDI_interface_t;
/**@}*/

/**
 * @defgroup MDI_Platform MDI API MDD Platform Object
 *
 * Proxy to a MDD Platform description.
 */
/**@{*/
/**
 * @brief Platform class abstraction
 *
 * Corresponds to the MDD Platform object.
 * A MDI_interface_t instance corresponds
 * to 1 and only one platform.
 * When several platforms are supported, the
 * MDI interface factory parameter should
 * be used by the client to select the
 * right target platform.
 */
typedef MDI_object_t MDI_Platform_t;
/**@}*/

/**
 * @defgroup MDI_Processor MDI API MDD Processor Object
 *
 * Proxy to a MDD Processor description.
 */
/**@{*/
/**
 * @brief Processor class abstraction
 *
 * A MDI_interface_t refer to a single Platform
 * which itself may be implemented by several
 * processors. Some encoder, decoder and
 * compiler back-end informations may depend upon
 * the actual target processor selected.
 * Some informations may have to be interpreted
 * differently from one processor to the other.
 */
typedef MDI_object_t MDI_Processor_t;
/**@}*/

/**
 * @defgroup MDI_Instruction MDI API MDD Instruction Object
 *
 * Proxy to a MDD Instruction description.
 */
/**@{*/
/**
 * @brief Instruction class abstraction
 *
 * The assembly level abstraction of architecture
 * instructions.
 */
typedef MDI_object_t MDI_Instruction_t;
/**@}*/

/**
 * @defgroup MDI_Opcode MDI API MDD Opcode Object
 *
 * Proxy to a MDD Opcode description.
 */
/**@{*/
/**
 * @brief Opcode class abstraction
 *
 * Should give more information on encoding
 * for an Instruction when multiple
 * processors of the same family are implemented.
 * Otherwise there is a 1-1 match with Instruction.
 */
typedef MDI_object_t MDI_Opcode_t;
/**@}*/

/**
 * @defgroup MDI_Operator MDI API MDD Operator Object
 *
 * Proxy to a MDD Operator description.
 */
/**@{*/
/**
 * @brief Operator class abstraction
 *
 * A specialisation of Instruction for
 * compiler backends, plus some compiler
 * specific operators when needed.
 * Generally there is a 1-1 match with Instruction.
 */
typedef MDI_object_t MDI_Operator_t;
/**@}*/

/**
 * @addtogroup MDI_interface
 */
/**@{*/

/**
 * @brief Create a new MDI interface for some platform.
 *
 * An interface instance must be obtained through this constructor.
 *
 * @param self_ref A reference to an interface object to construct.
 * @param params An abstrat object for to select a specialisation, may be @c NULL.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_interface_init(MDI_interface_t *self_ref, MDI_object_t params);

/**
 * @brief Destroy a MDI interface.
 *
 * An interface instance must be released through this destructor.
 * It is invalid to call this function for a non initialized interface
 * or to call it twice.
 *
 * @param self_ref A reference to a valid interface object.
 * @return 0 on success, failure otherwise.
 */
MDI_INTERFACE MDI_res_t MDI_interface_fini(MDI_interface_t *self_ref);

/**
 * @brief Returns the implemented MDI API revision
 *
 * It is the responsibility of the client to verify whether the interface
 * matches as for instance with
 * @c MDI_VERSION_REV_COMPATIBLE(MDI_VERSION_REV,MDI_interface_revision(self)).
 *
 * @param self A valid interface object.
 * @return The MDI API full revision.
 */
MDI_INTERFACE MDI_rev_t MDI_interface_revision(MDI_interface_t self);
/**@}*/


/**
 * @addtogroup MDI_Platform
 */
/**@{*/

/**
 * @brief Implemented Platform.
 *
 * Access to the Platform implemented by the interface instance.
 *
 * @param self The interface instance.
 * @return A valid platform.
 */
MDI_INTERFACE MDI_Platform_t MDI_Platform(MDI_interface_t self);

/**
 * @brief Platform ID
 *
 * Returns the MDD Platform unique string identifier.
 *
 * @param self The Platform instance.
 * @return A unique non NULL platform identifier.
 */
MDI_INTERFACE MDI_str_t MDI_Platform_ID(MDI_Platform_t self);

/**
 * @brief Platform implemented processors
 *
 * Returns the list of implemented processors for the platform.
 *
 * @param self The Platform instance.
 * @return A space separated list of processor IDs or NULL.
 */
MDI_INTERFACE MDI_str_t MDI_Platform_processors(MDI_Platform_t self);
/**@}*/

/**
 * @addtogroup MDI_Instruction
 */
/**@{*/

/**
 * @brief Available instructions descriptions count.
 *
 * Returns the number of available Instructions for the Platform.
 * To be used for iteration with MDI_Instructions_iter().
 *
 * Note that depending on the source of the machine description,
 * the same architectural level description may appear several time
 * as distinct Instructions.
 * This is the case for instance when several cores from the same
 * family are merged into a single description.
 * This is also the case when different cores implement different
 * encoding or scheduling for the same architectural instruction.
 * Generally each of the instruction for each core has an
 * associated Opcode. and the Opcodes for all cores are merged into
 * a single Operator..
 * 
 *
 * @param self The interface instance.
 * @return The Instructions count.
 */
MDI_INTERFACE MDI_idx_t MDI_Instructions_count(MDI_interface_t self);

/**
 * @brief Get Instruction description by index.
 *
 * Returns the Instruction through dense index.
 *
 * @param self The interface instance.
 * @param instruction_idx The Instruction index in [0, MDI_Instructions_count()[.
 * @return The Instruction description.
 */
MDI_INTERFACE MDI_Instruction_t MDI_Instructions_iter(MDI_interface_t self, MDI_idx_t instruction_idx);

/**
 * @brief Get Instruction index.
 *
 * Returns the Instruction index.
 *
 * @param self The Instruction.
 * @return The Instruction dense index.
 */
MDI_INTERFACE MDI_idx_t MDI_Instruction_idx(MDI_Instruction_t self);

/**
 * @brief Get Instruction unique ID.
 *
 * Returns the Instruction unique string identifier.
 *
 * @param self The Instruction.
 * @return The Instruction identifier.
 */
MDI_INTERFACE MDI_str_t MDI_Instruction_ID(MDI_Instruction_t self);

/**
 * @brief Get Instruction menmonic.
 *
 * Returns the Instruction mnemnonic string.
 * The assembly level short name for the instruction.
 * Note that several Instruction descriptions may have the same mnemonic.
 *
 * @param self The Instruction.
 * @return The mnemonic string.
 */
MDI_INTERFACE MDI_str_t MDI_Instruction_mnemonic(MDI_Instruction_t self);

/**
 * @brief Get Instruction properties.
 *
 * Returns the Instruction properties list.
 * The properties are arbitrate identifiers available for architectural
 * instructions classification.
 * There is no guarantee that properties have an uniform semantic
 * between several descriptions.
 *
 * Also there is no iterator for properties which are not formally
 * defined in the machine description. These are just like tags.
 * Hence a client may have to iterate over all properties of all
 * instructions to discover all available properties for a specific
 * description.
 *
 * @param self The Instruction.
 * @return The properties string as a space separate list of identifiers. 
 */
MDI_INTERFACE MDI_str_t MDI_Instruction_properties(MDI_Instruction_t self);
/**@}*/

/**
 * @addtogroup MDI_Opcode
 */
/**@{*/

/**
 * @brief Opcode count.
 *
 * Returns the number of available Opcodes for the Platform.
 * To be used for iteration with MDI_Opcodes_iter().
 *
 * @param self The interface instance.
 * @return The Opcodes  count.
 */
MDI_INTERFACE MDI_idx_t MDI_Opcodes_count(MDI_interface_t self);

/**
 * @brief Get Opcode by index.
 *
 * Returns the Opcode type through dense index.
 *
 * @param self The interface instance.
 * @param opcode_idx The Opcode index in [0, MDI_Opcodes_count()[.
 * @return The Opcode.
 */
MDI_INTERFACE MDI_Opcode_t MDI_Opcodes_iter(MDI_interface_t self, MDI_idx_t opcode_idx);

/**
 * @brief Get Opcode index.
 *
 * Returns the Opcode index.
 *
 * @param self The Opcode.
 * @return The Opcode dense index.
 */
MDI_INTERFACE MDI_idx_t MDI_Opcode_idx(MDI_Opcode_t self);

/**
 * @brief Get Opcode unique ID.
 *
 * Returns the Opcode unique string identifier.
 *
 * @param self The Opcode.
 * @return The Opcode identifier.
 */
MDI_INTERFACE MDI_str_t MDI_Opcode_ID(MDI_Opcode_t self);

/**
 * @brief Get the Instruction decoded through this opcode
 *
 * Returns the matching Instruction for the Opcode.
 *
 * @param self The Opcode.
 * @return The associated Instruction.
 */
MDI_INTERFACE MDI_Instruction_t MDI_Opcode_instruction(MDI_Opcode_t self);
/**@}*/

/**
 * @addtogroup MDI_Operator
 */
/**@{*/

/**
 * @brief Operator count.
 *
 * Returns the number of available Operators for the Platform.
 * To be used for iteration with MDI_Operators_iter().
 *
 * @param self The interface instance.
 * @return The Opcodes count.
 */
MDI_INTERFACE MDI_idx_t MDI_Operators_count(MDI_interface_t self);
/**
 * @brief Get Operator by index.
 *
 * Returns the Operator through dense index.
 *
 * @param self The interface instance.
 * @param operator_idx The Operator index in [0, MDI_Operators_count()[.
 * @return The Operator.
 */
MDI_INTERFACE MDI_Operator_t MDI_Operators_iter(MDI_interface_t self, MDI_idx_t operator_idx);
/**
 * @brief Get Operator index.
 *
 * Returns the Operator index.
 *
 * @param self The Operator.
 * @return The Operator dense index.
 */
MDI_INTERFACE MDI_idx_t MDI_Operator_idx(MDI_Operator_t self);

/**
 * @brief Get Operator unique ID.
 *
 * Returns the Operator unique string identifier.
 *
 * @param self The Operator.
 * @return The Operator identifier.
 */
MDI_INTERFACE MDI_str_t MDI_Operator_ID(MDI_Operator_t self);

/**
 * @brief Get the Operator Opcode for a given Processor
 *
 * Returns the Opcode corresponding to this Operator for
 * a given Processor.
 *
 * @param self The Operator.
 * @param processor The target Processor.
 * @return The associated Opcode for the target Processor.
 */
MDI_INTERFACE MDI_Instruction_t MDI_Operator_opcode(MDI_Operator_t self, MDI_Processor_t processor);
/**@}*/

/**
 * @brief Get the Operator attributes
 *
 * Returns the Operator attributes list.
 * The attributes are arbitrate identifiers available for processing
 * by code generation and analysis tools.
 * There is no guarantee that attributes have an uniform semantic
 * between several descriptions.
 *
 * Compared to Instruction properties, Operator attributes are
 * generally inferred from Operators semantic (i.e. genrated),
 * while properties are part of the architectural description
 * input (i.e. authored).
 * Though, as for properties which are used for Instructions
 * classification, attributes can also be used to classify Operators
 * (hence also the associated opcodes and instructions).
 *
 * There is no iterator for attributes which are not formally
 * defined in the machine description. These are just like tags.
 * Hence a client may have to iterate over all attributes of all
 * operators to discover all available attributes for a specific
 * description.
 *
 * @param self The Operator.
 * @return The attributes space separated string.
 */
    MDI_INTERFACE MDI_str_t MDI_Operator_attributes(MDI_Operator_t self);
/**@}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MDI_H_INCLUDED */
